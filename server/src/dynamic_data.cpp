#include <dynamic_data.h>

#include <auth_service.h>
#include <log_service.h>
#include <method_service.h>
#include <web_service.h>
#include <web_service/connection.h>

namespace abcserver
{

namespace dynamic_data
{

void register_(AuthService& auth_service, std::shared_ptr<web_service::Connection> connection, const Request& request)
{
    auto json = boost::json::parse(request.Body());

    if(!json.is_object())
        throw std::invalid_argument{ "Params is not a JSON object" };

    const auto& object = json.as_object();

    auto* login_ptr = object.if_contains("login");

    if(login_ptr == nullptr)
        throw std::invalid_argument{ "Request does not contain login" };

    auto* password_ptr = object.if_contains("password");

    if(password_ptr == nullptr)
        throw std::invalid_argument{ "Request does not contain password" };

    auto* login = login_ptr->if_string();

    if(login == nullptr)
        throw std::invalid_argument{ "Login is not string" };

    auto* password = password_ptr->if_string();

    if(password == nullptr)
        throw std::invalid_argument{ "Password is not string" };

    IPAddress ip_address{connection->Socket().remote_endpoint().address().to_string()};

    auth_service.CreateSession2(login->c_str(), password->c_str(), ip_address, [connection](Session::UserIDType id, Token token, Error error) {
        if(error)
        {
            connection->SendResponse(CreateErrorResponse(error.message()));
            return;
        }

        boost::json::object result =
        {
            {"success", id != Session::invalid_user_id}
        };

        Response response;
        response.SetStatus(Response::Status::ok);

        if(!token.IsNull())
            response.SetCookie("tkn", token.ToString(), 86400);

        response.SetBody(boost::json::serialize(result));

        connection->SendResponse(std::move(response));
    });
}

void authorize(AuthService& auth_service, std::shared_ptr<web_service::Connection> connection, const Request& request)
{
    auto json = boost::json::parse(request.Body());

    if(!json.is_object())
        throw std::invalid_argument{ "Params is not a JSON object" };

    const auto& object = json.as_object();

    auto* login_ptr = object.if_contains("login");

    if(login_ptr == nullptr)
        throw std::invalid_argument{ "Request does not contain login" };

    auto* password_ptr = object.if_contains("password");

    if(password_ptr == nullptr)
        throw std::invalid_argument{ "Request does not contain password" };

    auto* login = login_ptr->if_string();

    if(login == nullptr)
        throw std::invalid_argument{ "Login is not string" };

    auto* password = password_ptr->if_string();

    if(password == nullptr)
        throw std::invalid_argument{ "Password is not string" };

    IPAddress ip_address{connection->Socket().remote_endpoint().address().to_string()};

    auth_service.CreateSession(login->c_str(), password->c_str(), ip_address, [connection](Session::UserIDType id, Token token, Error error) {
        if(error)
        {
            connection->SendResponse(CreateErrorResponse(error.message()));
            return;
        }

        boost::json::object result =
        {
            {"success", id != Session::invalid_user_id}
        };

        Response response;
        response.SetStatus(Response::Status::ok);

        if(!token.IsNull())
            response.SetCookie("tkn", token.ToString(), 86400);

        response.SetBody(boost::json::serialize(result));

        connection->SendResponse(std::move(response));
    });
}

void exit(AuthService& auth_service, std::shared_ptr<web_service::Connection> connection, const Request& request)
{
    auto token = request.Cookie("tkn");

    auth_service.RemoveSession(token);

    boost::json::object result =
    {
        {"success", true}
    };

    Response response;
    response.SetStatus(Response::Status::ok);
    response.SetCookie("tkn", "deleted", -100);
    response.SetBody(boost::json::serialize(result));

    connection->SendResponse(std::move(response));
}

void process(WebService& web_service, std::shared_ptr<web_service::Connection> connection, const Request& request)
{
    auto* log_service      = web_service.LogService();

    auto* auth_service     = web_service.AuthService();

    auto* method_service   = web_service.MethodService();

    auto* database_service = web_service.DatabaseService();

    auto xmethod = request.XMethod();

    if(log_service)
        log_service->Info("Method: %1%, Body: %2%", xmethod, request.Body());

    if(xmethod.empty())
    {
        throw std::invalid_argument{"Method is not set"};
    }
    if(xmethod == "Reg")
    {
        register_(*auth_service, connection, request);
        return;
    }
    if(xmethod == "Auth")
    {
        authorize(*auth_service, connection, request);
        return;
    }
    if(xmethod == "Exit")
    {
        exit(*auth_service, connection, request);
        return;
    }

    auto cookie = request.Cookie("tkn");

    if(cookie.empty())
    {
        boost::json::object result{};

        Response response;
        response.SetStatus(Response::Status::unauthorized);
        response.SetHeader(Response::Field::content_type, "application/javascript");
        response.SetCookie("tkn", "deleted", -100);
        response.SetBody(boost::json::serialize(result));

        connection->SendResponse(std::move(response));

        return;
    }

    auto token = request.Cookie("tkn");

    auto id = auth_service->FindUserID(token);

    if(id == Session::invalid_user_id)
    {
        boost::json::object result{};

        Response response;
        response.SetStatus(Response::Status::unauthorized);
        response.SetHeader(Response::Field::content_type, "application/javascript");
        response.SetCookie("tkn", "deleted", -100);
        response.SetBody(boost::json::serialize(result));

        connection->SendResponse(std::move(response));

        return;
    }

    auto params = boost::json::parse(request.Body());

    if(!params.is_object())
        throw std::invalid_argument{"Params is not a JSON object"};

    auto& handler = method_service->Method(xmethod);

    handler(*database_service, *connection, id, params.as_object(), [connection](boost::json::value result, Error error) {
        if(error)
        {
            connection->SendResponse(CreateErrorResponse(error.message()));
            return;
        }

        Response response;
        response.SetStatus(Response::Status::ok);
        response.SetBody(boost::json::serialize(result));

        connection->SendResponse(std::move(response));
    });
}

}
}
