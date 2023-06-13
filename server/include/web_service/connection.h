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

class Connection : public std::enable_shared_from_this<Connection>
{
public:
    static std::shared_ptr<Connection> Create(WebService& web_service);

    void Process();

    inline boost::asio::ip::tcp::socket& Socket() noexcept { return socket_; }

    void SendResponse(Response response);
private:
    Connection(WebService& web_service);

    void ProcessRequest();
private:
    WebService& web_service_;

    boost::asio::ip::tcp::socket socket_;

    boost::beast::flat_buffer buffer_;

    Request  request_;

    Response response_;
};

}

}
