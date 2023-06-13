#include <web_service/connection.h>

#include <log_service.h>
#include <web_service.h>

namespace abcserver
{

namespace web_service
{

std::shared_ptr<Connection> Connection::Create(WebService& web_service)
{
    return std::shared_ptr<Connection>{new Connection{web_service}};
}

void Connection::Process()
{
    boost::beast::http::async_read(socket_, buffer_, request_.InternalRequest(),
                                   [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if(ec)
        {
            if(auto* log = self->web_service_.LogService())
                log->Error("Read data error: %1% on %2%", ec.message(), self->socket_.remote_endpoint().address().to_string());
            return;
        }

        if(auto* log = self->web_service_.LogService())
            log->Info("Received %1% byte from %2%",  bytes_transferred, self->socket_.remote_endpoint().address().to_string());

        self->ProcessRequest();
    });
}

Connection::Connection(WebService& web_service) :
    web_service_{web_service}, socket_{web_service.IOService()}
{

}

void Connection::ProcessRequest()
{
    try
    {
        web_service_.RequestHandler()(web_service_, shared_from_this(), request_);
    }
    catch(const std::exception& ex)
    {
        if(auto* log = web_service_.LogService())
            log->Warning("Request error: %1%", ex.what());

        SendResponse(CreateErrorResponse(ex.what()));
    }
}

void Connection::SendResponse(Response response)
{
    response_ = std::move(response);

    auto handler = [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if(ec)
        {
            if(auto* log = self->web_service_.LogService())
                log->Error("Send data error: %1% on %2%", ec.message(), self->socket_.remote_endpoint().address().to_string());
            return;
        }

        if(auto* log = self->web_service_.LogService())
            log->Info("Sended %1% byte to %2%",  bytes_transferred, self->socket_.remote_endpoint().address().to_string());

        self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    };

    boost::beast::http::async_write(socket_, response_.InternalResponse(), std::move(handler));
}

}

}
