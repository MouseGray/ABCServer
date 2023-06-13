#pragma once

#include <cassert>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/optional/optional.hpp>

#include <libpq-fe.h>

#include <error.h>
#include <format.h>

#define NOEXCEPT_REQ

namespace abcserver
{

inline std::string StringCast(    std::nullptr_t value) { return "null"; }
inline std::string StringCast(              bool value) { return value ? "true" : "false"; }
inline std::string StringCast(              char value) { return { value }; }
inline std::string StringCast(       signed char value) { return std::to_string(value); }
inline std::string StringCast(     unsigned char value) { return std::to_string(value); }
inline std::string StringCast(        signed int value) { return std::to_string(value); }
inline std::string StringCast(      unsigned int value) { return std::to_string(value); }
inline std::string StringCast(       signed long value) { return std::to_string(value); }
inline std::string StringCast(     unsigned long value) { return std::to_string(value); }
inline std::string StringCast(  signed long long value) { return std::to_string(value); }
inline std::string StringCast(unsigned long long value) { return std::to_string(value); }
inline std::string StringCast(             float value) { return std::to_string(value); }
inline std::string StringCast(            double value) { return std::to_string(value); }
inline std::string StringCast(const std::string& value) { return value; }

inline void StringCast(const std::string& str,               bool& value) { value = !str.empty() && (str.front() == 'T' || str.front() == 't'); }
inline void StringCast(const std::string& str,               char& value) { value = !str.empty() ? str.front() : '\0'; }
inline void StringCast(const std::string& str,        signed char& value) { value = std::stoi(str); }
inline void StringCast(const std::string& str,      unsigned char& value) { value = std::stoi(str); }
inline void StringCast(const std::string& str,         signed int& value) { value = std::stoi(str); }
inline void StringCast(const std::string& str,       unsigned int& value) { value = std::stoi(str); }
inline void StringCast(const std::string& str,        signed long& value) { value = std::stol(str); }
inline void StringCast(const std::string& str,      unsigned long& value) { value = std::stol(str); }
inline void StringCast(const std::string& str,   signed long long& value) { value = std::stoll(str); }
inline void StringCast(const std::string& str, unsigned long long& value) { value = std::stoll(str); }
inline void StringCast(const std::string& str,              float& value) { value = std::stof(str); }
inline void StringCast(const std::string& str,             double& value) { value = std::stod(str); }
inline void StringCast(const std::string& str,        std::string& value) { value = str; }

class Field;
class Row;
class Result;

class Field
{
    friend class Row;
public:
    template<typename T>
    boost::optional<T> Optional()
    {
        if(PQgetisnull(result_, row_, column_))
            return {};

        T value;
        StringCast(PQgetvalue(result_, row_, column_), value);

        return { value };
    }

    bool IsNull()
    {
        return PQgetisnull(result_, row_, column_);
    }

    template<typename T>
    T To()
    {
        T value;
        StringCast(PQgetvalue(result_, row_, column_), value);

        return { value };
    }
private:
    Field(const PGresult* result, std::size_t row, std::size_t column) :
        result_{result}, row_{row}, column_{column}
    {

    }

    const PGresult* result_;
    std::size_t     row_;
    std::size_t     column_;
};

class Row
{
    friend class Result;
public:
    Field operator[](std::size_t column)
    {
        int size = PQnfields(result_);

        if(column >= size)
            throw std::invalid_argument{utils::format("column '%1%' out of range (max=%2%)", column, size)};

        return {result_, row_, column};
    }

    Field operator[](const std::string& name)
    {
        int column = PQfnumber(result_, name.c_str());

        if(column == -1)
            throw std::invalid_argument{utils::format("column '%1%' not found", name)};

        return {result_, row_, static_cast<std::size_t>(column)};
    }
private:
    Row(const PGresult* result, std::size_t row) :
        result_{result}, row_{row}
    {

    }

    const PGresult* result_;
    std::size_t     row_;
};

class Result
{
    friend class Connection;

    struct ResultDeleter
    {
        void operator()(PGresult* result) noexcept { PQclear(result); }
    };
public:
    Row operator[](std::size_t row)
    {
        const PGresult* result       = nullptr;
        int             total_size   = 0;
        std::size_t     internal_row = 0;

        for(const auto& r : data_)
        {
            total_size += PQntuples(r.get());

            if(total_size > row)
            {
                result = r.get();
                internal_row = row - internal_row;
                break;
            }

            internal_row = total_size;
        }

        if(result == nullptr)
            throw std::invalid_argument{utils::format("row '%1%' out of range", row)};

        return {result, internal_row};
    }

    void Append(PGresult* result)
    {
        data_.emplace_back(result);
    }

    std::size_t Size() const noexcept
    {
        int total_size = 0;

        for(const auto& r : data_)
            total_size += PQntuples(r.get());

        return total_size;
    }

    bool HasError() const noexcept
    {
        for(const auto& r : data_)
        {
            auto status = PQresultStatus(r.get());
            if(status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
                return true;
        }

        return false;
    }

    void Clear() noexcept
    {
        data_.clear();
    };
private:
    Result() = default;

    std::vector<std::unique_ptr<PGresult, ResultDeleter>> data_;
};

class ParameterPack
{
public:
    ParameterPack();

    template<typename Ty>
    void Append(Ty&& param)
    {
        data_.push_back( StringCast(param) );
        is_modified = true;
    }

    const char* const* Values() const;

    const int* Lengths() const;

    int Size() const noexcept;
private:
    void Modify() const;

    mutable const char* const* values_;
    mutable const int*         lengths_;
    mutable bool               is_modified;
    std::vector<std::string>   data_;
};

class Database;

class Connection
{
    struct ConnectionDeleter
    {
        void operator()(PGconn* connection) noexcept { PQfinish(connection); }
    };
public:
    using CallbackType = std::function<void(Result*, Error)>;

    Connection(Database* database, const std::string& host, const std::string& port, const std::string& dbname,
               const std::string& user, const std::string& pass);

    inline bool IsReady() const noexcept { return is_ready_; }

    void AsyncExec(const std::string& query, const ParameterPack& params, std::function<void(Result*, Error)> callback) NOEXCEPT_REQ;
private:
    void SendQuery(const std::string& query, const ParameterPack& params);

    void ReadHanler(const boost::system::error_code& error) NOEXCEPT_REQ;

    boost::asio::ip::tcp::socket               socket_;
    std::unique_ptr<PGconn, ConnectionDeleter> connection_;
    bool                                       is_ready_;
    Result                                     result_;
    CallbackType                               callback_;
    Database*                                  database_;
};

class Database
{
    class QueryPack
    {
    public:
        QueryPack(const std::string& query, const ParameterPack& params, const Connection::CallbackType& callback) :
            query_{query}, params_{params}, callback_{callback} {};

        inline const std::string& Query() const noexcept { return query_; }

        inline const ParameterPack& Params() const noexcept { return params_; }

        inline const Connection::CallbackType& Callback() const noexcept { return callback_; }
    private:
        std::string              query_;
        ParameterPack            params_;
        Connection::CallbackType callback_;
    };
public:
    Database(boost::asio::io_service& io_service, const std::string& host, const std::string& port,
             const std::string& dbname, const std::string& user, const std::string& pass,
             std::size_t count);

    inline boost::asio::io_service& IOService() noexcept { return io_service_; }

    void AsyncExec(const std::string& query, const ParameterPack& params, const Connection::CallbackType& callback);

    void ExecNext(Connection* connection) NOEXCEPT_REQ;
private:
    boost::asio::io_service& io_service_;
    std::mutex               mutex_;
    std::vector<Connection>  connections_;
    std::queue<QueryPack>    queries_;
};

class LogService;

class DatabaseService
{
public:
    DatabaseService(boost::asio::io_service& io_service) :
        io_service_{io_service} {}

    void AppendDatabase(const std::string& host, const std::string& port, const std::string& dbname,
                        const std::string& user, const std::string& pass, std::size_t count = 1);

    void AsyncExec(const std::string& dbname, const std::string& query, const ParameterPack& params,
                   const Connection::CallbackType& callback);

    void AsyncExec(const std::string& query, const ParameterPack& params,
                   const Connection::CallbackType& callback);
private:
    boost::asio::io_service&                                   io_service_;
    std::unordered_map<std::string, std::unique_ptr<Database>> databases_;
};

}
