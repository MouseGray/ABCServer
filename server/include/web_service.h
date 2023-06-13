#pragma once

#include <thread>
#include <vector>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>

namespace abcserver
{

class AuthService;
class ConfigService;
class DatabaseService;
class LogService;
class MethodService;

namespace web_service
{
class Connection;
}

class Request;
class Response;

class WebService
{
public:
    using RequestHandlerType = std::function<void(WebService&, std::shared_ptr<web_service::Connection>, const Request&)>;
public:
    WebService(boost::asio::io_service& io_service,
               boost::asio::ip::port_type port,
               boost::asio::ip::port_type ws_port);

    void Run();

    void Stop();

    void SetLogService(class LogService* log_service) noexcept;

    class LogService* LogService() noexcept;

    void SetConfigService(class ConfigService* config_service) noexcept;

    class ConfigService* ConfigService() noexcept;

    void SetAuthService(class AuthService* auth_service) noexcept;

    class AuthService* AuthService() noexcept;

    void SetMethodService(class MethodService* method_service) noexcept;

    class MethodService* MethodService() noexcept;

    void SetDatabaseService(class DatabaseService* database_service) noexcept;

    class DatabaseService* DatabaseService() noexcept;

    void SetRequestHandler(RequestHandlerType request_handler) noexcept;

    RequestHandlerType& RequestHandler() noexcept;

    boost::asio::io_service& IOService();
private:
    void Accept();

    void WSAccept();

    class AuthService* auth_service_;

    class ConfigService* config_service_;

    class DatabaseService* database_service_;

    class LogService* log_service_;

    class MethodService* method_service_;

    boost::asio::io_service& io_service_;

    boost::asio::ip::tcp::acceptor acceptor_;

    boost::asio::ip::tcp::acceptor ws_acceptor_;

    std::vector<std::thread> threads_;

    RequestHandlerType request_handler_;
};

}
