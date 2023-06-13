#include <web_service.h>

#include <boost/json/src.hpp>

#include <config_service.h>
#include <log_service.h>

#include <web_service/connection.h>
#include <web_service/ws_connection.h>

namespace abcserver
{

WebService::WebService(boost::asio::io_service& io_service,
                       boost::asio::ip::port_type port,
                       boost::asio::ip::port_type ws_port) :
    auth_service_{},
    config_service_{},
    database_service_{},
    log_service_{},
    method_service_{},
    io_service_{io_service},
    acceptor_{io_service,
              boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), port}},
    ws_acceptor_{io_service,
                 boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), ws_port}},
    request_handler_{}
{

}

void WebService::Run()
{
    Accept();

    WSAccept();

    if(log_service_)
        log_service_->Info("Worker thread %#x started", std::this_thread::get_id());

    while (true) {
        try {
            io_service_.run();
            return;
        }
        catch(...) {
            if(log_service_)
                log_service_->Info("Worker thread %1% restored", std::this_thread::get_id());
        }
    }
}

void WebService::Stop()
{
    io_service_.stop();
}

void WebService::SetLogService(class LogService* log_service) noexcept
{
    log_service_ = log_service;
}

class LogService* WebService::LogService() noexcept
{
    return log_service_;
}

void WebService::SetConfigService(class ConfigService* config_service) noexcept
{
    config_service_ = config_service;
}

class ConfigService* WebService::ConfigService() noexcept
{
    return config_service_;
}

void WebService::SetAuthService(class AuthService* auth_service) noexcept
{
    auth_service_ = auth_service;
}

class AuthService* WebService::AuthService() noexcept
{
    return auth_service_;
}

void WebService::SetMethodService(class MethodService* method_service) noexcept
{
    method_service_ = method_service;
}

MethodService* WebService::MethodService() noexcept
{
    return method_service_;
}

void WebService::SetDatabaseService(class DatabaseService* database_service) noexcept
{
    database_service_ = database_service;
}

class DatabaseService* WebService::DatabaseService() noexcept
{
    return database_service_;
}

void WebService::SetRequestHandler(RequestHandlerType request_handler) noexcept
{
    request_handler_ = std::move(request_handler);
}

WebService::RequestHandlerType& WebService::RequestHandler() noexcept
{
    return request_handler_;
}

boost::asio::io_service& WebService::IOService()
{
    return io_service_;
}

void WebService::Accept()
{
    auto new_connection = web_service::Connection::Create(*this);

    acceptor_.async_accept(new_connection->Socket(),
                           [this, connection = new_connection](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            connection->Process();
        }
        else
        {
            if(log_service_)
                log_service_->Error("Accept connection error: %1%", ec.message());
        }

        Accept();
    });
}

void WebService::WSAccept()
{
    auto new_ws_connection = web_service::WSConnection::Create(*this);

    ws_acceptor_.async_accept(new_ws_connection->Socket().next_layer().socket(),
                              [this, connection = new_ws_connection](const boost::system::error_code& ec)
    {
        if (!ec)
        {
            connection->Process();
        }
        else
        {
            if(log_service_)
                log_service_->Error("Accept connection error: %1%", ec.message());
        }

        WSAccept();
    });
}

}
