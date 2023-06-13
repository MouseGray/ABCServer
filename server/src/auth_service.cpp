#include <auth_service.h>

#include <cstring>
#include <stdexcept>

#include <database_service.h>

namespace abcserver
{

AuthService::AuthService(boost::asio::io_service& io_service) :
    timer_{io_service}, database_service_{}
{

}

void AuthService::SetDatabaseService(DatabaseService* database_service) noexcept
{
    database_service_ = database_service;
}

void AuthService::Run()
{
    timer_.expires_after(boost::asio::chrono::seconds(+Session::max_deadline));
    timer_.async_wait(std::bind(&AuthService::TimerHandler, this, std::placeholders::_1));
}

void AuthService::Stop()
{
    timer_.cancel();
}

void AuthService::CreateSession(const std::string& login, const std::string& password, IPAddress ip_address,
                                std::function<void(Session::UserIDType, Token, Error)> callback)
{
    assert(database_service_);

    constexpr auto query = u8R"(
SELECT
  "id"
FROM
  "user"
WHERE
  "login" = $1::text
  AND
  "password" = crypt($2::text, "password")
)";

    ParameterPack params;
    params.Append(login);
    params.Append(password);

    database_service_->AsyncExec(query, params, [this, ip_address, callback](Result* result, Error error) {
        if(error)
        {
            callback(Session::invalid_user_id, Token{}, error);
            return;
        }

        if(result->Size() == 0)
        {
            callback(Session::invalid_user_id, Token{}, Error::NoError());
            return;
        }

        auto id = (*result)[0]["id"].To<int>();

        auto token = AddSession(id, ip_address);

        callback(id, std::move(token), Error::NoError());
    });
}

void AuthService::CreateSession2(const std::string& login, const std::string& password,
                                 IPAddress ip_address, std::function<void (Session::UserIDType, Token, Error)> callback)
{
    assert(database_service_);

    constexpr auto query = u8R"(
INSERT INTO
  "user"
  (
    "login",
    "password"
  )
VALUES
(
  $1::text,
  crypt($2::text, gen_salt('bf'))
)
RETURNING
  "id"
)";

    ParameterPack params;
    params.Append(login);
    params.Append(password);

    database_service_->AsyncExec(query, params, [this, ip_address, callback](Result* result, Error error) {
        if(error)
        {
            callback(Session::invalid_user_id, Token{}, error);
            return;
        }

        if(result->Size() == 0)
        {
            callback(Session::invalid_user_id, Token{}, Error::NoError());
            return;
        }

        auto id = (*result)[0]["id"].To<int>();

        auto token = AddSession(id, ip_address);

        callback(id, std::move(token), Error::NoError());
    });
}

void AuthService::RemoveSession(const Token& token)
{
    std::lock_guard<std::mutex> lock{mutex_};

    auto range = std::equal_range(std::begin(sessions_), std::end(sessions_), token, Comparator{});

    if(range.first == range.second)
        return;

    sessions_.erase(range.first, range.second);
}

Session::UserIDType AuthService::FindUserID(const Token& token)
{
    std::lock_guard<std::mutex> lock{mutex_};

    auto* session = FindSession(token);

    if(session == nullptr)
        return Session::invalid_user_id;

    session->Extend();

    return session->UserID();
}

Session* AuthService::FindSession(const Token& token) noexcept
{
    auto range = std::equal_range(std::begin(sessions_), std::end(sessions_), token, Comparator{});

    if(range.first == range.second)
        return nullptr;

    return &(*range.first);
}

Token AuthService::AddSession(Session::UserIDType id, IPAddress ip_address)
{
    std::lock_guard<std::mutex> lock{mutex_};

    for(auto attempt = 0; attempt < max_attempt; ++attempt)
    {
        auto token = Token::GenerateToken();

        auto* session = FindSession(token);

        if(session == nullptr)
        {
            Session session{id, ip_address, token};

            auto it = std::lower_bound(std::begin(sessions_), std::end(sessions_), token, Comparator{});

            sessions_.insert(it, std::move(session));

            return token;
        }
    }

    throw std::invalid_argument{"All attempts to create a token have been exhausted"};
}

void AuthService::Clear()
{
    std::lock_guard<std::mutex> lock{mutex_};

    sessions_.erase(std::remove_if(std::begin(sessions_), std::end(sessions_), [](const Session& session) {
        return session.IsOverdue();
    }), std::end(sessions_));
}

void AuthService::TimerHandler(const boost::system::error_code& error)
{
    if(error)
        return;

    Clear();

    timer_.expires_after(boost::asio::chrono::seconds(+Session::max_deadline));
    timer_.async_wait(std::bind(&AuthService::TimerHandler, this, std::placeholders::_1));
}

}
