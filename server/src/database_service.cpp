#include <database_service.h>

#include <format.h>

namespace abcserver
{

ParameterPack::ParameterPack() :
    values_{}, lengths_{}, is_modified{true}
{

}

const char* const* ParameterPack::Values() const
{
    if(is_modified)
        Modify();

    return values_;
}

const int* ParameterPack::Lengths() const
{
    if(is_modified)
        Modify();

    return lengths_;
}

int ParameterPack::Size() const noexcept
{
    return data_.size();
}

void ParameterPack::Modify() const
{
    if(data_.empty())
    {
        values_     = nullptr;
        lengths_    = nullptr;
        is_modified = false;
        return;
    }

    const char** new_values_  = new const char*[data_.size()];
    int*         new_lengths_ = new int[data_.size()];

    for(std::size_t i = 0; i < data_.size(); ++i)
    {
        if(data_[i] == "null")
        {
            new_values_[i]  = nullptr;
            new_lengths_[i] = 0;
        }
        else
        {
            new_values_[i]  = data_[i].c_str();
            new_lengths_[i] = data_[i].size();
        }
    }

    if(values_ != nullptr)
        delete[] values_;

    if(lengths_ != nullptr)
        delete[] lengths_;

    values_     = new_values_;
    lengths_    = new_lengths_;
    is_modified = false;
}

Connection::Connection(Database* database, const std::string& host, const std::string& port, const std::string& dbname,
                       const std::string& user, const std::string& pass) :
    socket_{database->IOService()}, database_{database}
{
    std::string connection_string = utils::format("host=%1% port=%2% dbname=%3% user=%4% password=%5% "
                                                  "connect_timeout=10 client_encoding=utf-8",
                                                  host, port, dbname, user, pass);

    connection_ = std::unique_ptr<PGconn, ConnectionDeleter>{PQconnectdb(connection_string.c_str())};

    if(!connection_)
        throw std::bad_alloc{};

    if(PQstatus(connection_.get()) != CONNECTION_OK)
        throw std::invalid_argument{PQerrorMessage(connection_.get())};

    socket_.assign(boost::asio::ip::tcp::v4(), PQsocket(connection_.get()));

    is_ready_ = true;
}

void Connection::AsyncExec(const std::string& query, const ParameterPack& params, std::function<void (Result*, Error)> callback) NOEXCEPT_REQ
{
    if(!is_ready_)
    {
        callback(nullptr, Error::FatalError("Connection is busy"));
        return;
    }

    callback_ = callback;

    SendQuery(query, params);
}

void Connection::SendQuery(const std::string& query, const ParameterPack& params)
{
    if(params.Size() == 0)
    {
        if(!PQsendQuery(connection_.get(), query.c_str()))
        {
            callback_(nullptr, Error::UserError(PQerrorMessage(connection_.get())));
            return;
        }
    }
    else
    {
        if(!PQsendQueryParams(connection_.get(), query.c_str(), params.Size(), nullptr,
                              params.Values(), params.Lengths(), nullptr, 0))
        {
            callback_(nullptr, Error::UserError(PQerrorMessage(connection_.get())));
            return;
        }
    }

    socket_.async_wait(boost::asio::ip::tcp::socket::wait_read, std::bind(&Connection::ReadHanler, this, std::placeholders::_1));

    is_ready_ = false;
}

void Connection::ReadHanler(const boost::system::error_code& error) NOEXCEPT_REQ
{
    if(error)
    {
        callback_(nullptr, Error::UserError(error.message()));

        result_.Clear();
        is_ready_ = true;

        if(database_)
            database_->ExecNext(this);

        return;
    }

    if(!PQconsumeInput(connection_.get()))
    {
        callback_(nullptr, Error::UserError(PQerrorMessage(connection_.get())));

        result_.Clear();
        is_ready_ = true;

        if(database_)
            database_->ExecNext(this);

        return;
    }

    while(!PQisBusy(connection_.get()))
    {
        auto* result = PQgetResult(connection_.get());

        if(!result)
        {
            if(result_.HasError())
                callback_(nullptr, Error::UserError(PQresultErrorMessage(result_.data_[0].get())));
            else
                callback_(&result_, Error::NoError());

            result_.Clear();
            is_ready_ = true;

            if(database_)
                database_->ExecNext(this);

            return;
        }

        try
        {
            result_.Append(result);
        }
        catch(const std::exception& ex)
        {
            callback_(nullptr, Error::UserError(ex.what()));

            result_.Clear();
            is_ready_ = true;

            if(database_)
                database_->ExecNext(this);

            return;
        }
    }

    socket_.async_wait(boost::asio::ip::tcp::socket::wait_read, std::bind(&Connection::ReadHanler, this, std::placeholders::_1));
}

Database::Database(boost::asio::io_service& io_service, const std::string& host, const std::string& port,
                   const std::string& dbname, const std::string& user, const std::string& pass, std::size_t count) :
    io_service_{io_service}, connections_{}, queries_{}
{
    for(std::size_t i = 0; i < count; ++i)
        connections_.emplace_back(this, host, port, dbname, user, pass);
}

void Database::AsyncExec(const std::string& query, const ParameterPack& params, const Connection::CallbackType& callback)
{
    std::lock_guard<std::mutex> _{mutex_};

    for(auto& connection : connections_)
    {
        if(connection.IsReady())
        {
            connection.AsyncExec(query, params, callback);
            return;
        }
    }

    queries_.emplace(query, params, callback);
}

void Database::ExecNext(Connection* connection)
{
    std::lock_guard<std::mutex> _{mutex_};

    if(queries_.empty())
        return;

    const auto& query_pack = queries_.front();

    connection->AsyncExec(query_pack.Query(), query_pack.Params(), query_pack.Callback());

    queries_.pop();
}

void DatabaseService::AppendDatabase(const std::string& host, const std::string& port, const std::string& dbname,
                                     const std::string& user, const std::string& pass, std::size_t count)
{
    auto database = std::make_unique<Database>(io_service_, host, port, dbname, user, pass, count);
    databases_.emplace(dbname, std::move(database));
}

void DatabaseService::AsyncExec(const std::string& dbname, const std::string& query, const ParameterPack& params, const Connection::CallbackType& callback)
{
    auto it = databases_.find(dbname);

    if(it == std::end(databases_))
        throw std::invalid_argument{utils::format("database '%1%' is not exists", dbname)};

    it->second->AsyncExec(query, params, callback);
}

void DatabaseService::AsyncExec(const std::string& query, const ParameterPack& params,
                                const Connection::CallbackType& callback)
{
    if(databases_.empty())
        throw std::invalid_argument{"no databases"};

    databases_.begin()->second->AsyncExec(query, params, callback);
}

}
