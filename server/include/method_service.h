#pragma once

#include <string>
#include <unordered_map>

#include <boost/json.hpp>

#include <auth_service/session.h>
#include <error.h>

namespace abcserver
{

class DatabaseService;

namespace web_service
{
class Connection;
}

class MethodService
{
public:
    using CallbackType = std::function<void(boost::json::value result, Error error)>;
    using MethodType = std::function<void(DatabaseService&, web_service::Connection&, Session::UserIDType, const boost::json::object&, CallbackType)>;

    void LoadMethods();

    MethodType& Method(const std::string& method);
private:
    std::unordered_map<std::string, MethodType> methods_;
};

}
