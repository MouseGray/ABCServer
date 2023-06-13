#pragma once

#include <boost/beast.hpp>

#include <web_service/response.h>

namespace abcserver {

class WebService;

namespace web_service
{
class Connection;
}

class Request;

namespace dynamic_data {

void process(WebService& web_service, std::shared_ptr<web_service::Connection> connection, const Request& request);

};

}
