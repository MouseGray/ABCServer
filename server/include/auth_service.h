#pragma once

#include <thread>
#include <string>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

#include <error.h>

#include <auth_service/session.h>

namespace abcserver
{

class DatabaseService;

class AuthService
{
    static constexpr auto max_attempt = 5;

    struct Comparator
    {
        bool operator() (const Session& session, const Token& token) const { return session.Token() < token; }
        bool operator() (const Token& token, const Session& session) const { return token < session.Token(); }
    };
public:
    AuthService(boost::asio::io_service& io_service);

    void SetDatabaseService(DatabaseService* database_service) noexcept;

    void Run();

    void Stop();

    void CreateSession(const std::string& login, const std::string& password, IPAddress ip_address,
                       std::function<void(Session::UserIDType, Token, Error)> callback);

    void CreateSession2(const std::string& login, const std::string& password, IPAddress ip_address,
                        std::function<void(Session::UserIDType, Token, Error)> callback);

    void RemoveSession(const Token& token);

    Session::UserIDType FindUserID(const Token& token);
private:
    Session* FindSession(const Token& token) noexcept;

    Token AddSession(Session::UserIDType id, IPAddress ip_address);

    void Clear();

    void TimerHandler(const boost::system::error_code& error);

    std::vector<Session> sessions_;

    std::mutex mutex_;

    boost::asio::steady_timer timer_;

    DatabaseService* database_service_;
};

}
