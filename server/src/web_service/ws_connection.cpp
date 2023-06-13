#include <web_service/ws_connection.h>

#include <log_service.h>
#include <web_service.h>

namespace abcserver
{

namespace web_service
{

std::shared_ptr<WSConnection> WSConnection::Create(WebService& web_service)
{
    return std::shared_ptr<WSConnection>{new WSConnection{web_service}};
}

void WSConnection::Process()
{
    using namespace boost::beast;

    web_socket_.set_option(websocket::stream_base::timeout::suggested(role_type::server));

//    web_socket_.set_option(websocket::stream_base::decorator(
//                [](websocket::response_type& res)
//                {
//                    res.set(http::field::server,
//                        std::string(BOOST_BEAST_VERSION_STRING) +
//                            " websocket-server-async");
//                }));

    web_socket_.async_accept([self = shared_from_this()](error_code ec)
    {
        if(ec)
        {
            if(auto* log = self->web_service_.LogService())
                log->Error("Read data error: %1% on %2%", ec.message(), self->web_socket_.next_layer().socket().remote_endpoint().address().to_string());
            return;
        }

        self->ReceiveRequest();
    });
}

WSConnection::WSConnection(WebService& web_service) :
    web_service_{web_service}, web_socket_{web_service.IOService()}
{

}

void WSConnection::ProcessRequest()
{
    try
    {
        //web_service_.RequestHandler()(web_service_, shared_from_this(), request_);
    }
    catch(const std::exception& ex)
    {
        if(auto* log = web_service_.LogService())
            log->Warning("Request error: %1%", ex.what());

        SendResponse(CreateErrorResponse(ex.what()));
    }

    ReceiveRequest();
}

void WSConnection::ReceiveRequest()
{
    web_socket_.async_read(buffer_, [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if(ec)
        {
            if(auto* log = self->web_service_.LogService())
                log->Error("Read data error: %1% on %2%", ec.message(), self->web_socket_.next_layer().socket().remote_endpoint().address().to_string());
            return;
        }

        if(auto* log = self->web_service_.LogService())
            log->Info("Received %1% byte from %2%",  bytes_transferred, self->web_socket_.next_layer().socket().remote_endpoint().address().to_string());

        self->ProcessRequest();
    });
}

void WSConnection::SendResponse(Response response)
{
    //response_ = std::move(response);

    auto handler = [self = shared_from_this()](boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if(ec)
        {
            if(auto* log = self->web_service_.LogService())
                log->Error("Send data error: %1% on %2%", ec.message(), self->web_socket_.next_layer().socket().remote_endpoint().address().to_string());
            return;
        }

        if(auto* log = self->web_service_.LogService())
            log->Info("Sended %1% byte to %2%",  bytes_transferred, self->web_socket_.next_layer().socket().remote_endpoint().address().to_string());

        //self->socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
    };

    web_socket_.async_write(boost::asio::buffer(""), handler);
}

}

}
