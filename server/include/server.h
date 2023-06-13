#pragma once

#include <list>
#include <thread>

#include <boost/asio.hpp>

#include <auth/auth.h>

class server
{
public:
    server(unsigned short port);
    ~server();

    void exec(unsigned threads = 1);

    void stop();

    auth* auth_service();
private:
    void accept();

    void thread_exec();

    boost::asio::io_context io_context_;

    boost::asio::ip::tcp::acceptor acceptor_;

    std::list<std::thread> threads_;

    auth auth_;
};
