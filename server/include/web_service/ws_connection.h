#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <web_service/request.h>
#include <web_service/response.h>


namespace abcserver
{

class WebService;

namespace web_service
{

class WSConnection : public std::enable_shared_from_this<WSConnection>
{
public:
    static std::shared_ptr<WSConnection> Create(WebService& web_service);

    void Process();

    inline boost::beast::websocket::stream<boost::beast::tcp_stream>&
    Socket() noexcept { return web_socket_; }

    void SendResponse(Response response);
private:
    WSConnection(WebService& web_service);

    void ProcessRequest();

    void ReceiveRequest();
private:
    WebService& web_service_;

    boost::beast::websocket::stream<boost::beast::tcp_stream> web_socket_;

    boost::beast::flat_buffer buffer_;
};

}

}
