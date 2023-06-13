#include <web_service/response.h>

#include <boost/json/object.hpp>
#include <boost/json/serialize.hpp>

#include <format.h>

namespace abcserver
{

Response::InternalResponseType& Response::InternalResponse() noexcept
{
    return response_;
}

void Response::SetStatus(Status status)
{
    response_.result(status);
}

void Response::SetVersion(unsigned int version)
{
    response_.version(version);
}

void Response::SetHeader(const std::string& name, const std::string& value)
{
    response_.set(name, value);
}

void Response::SetHeader(Field name, const std::string& value)
{
    response_.set(name, value);
}

void Response::SetBody(std::string value)
{
    response_.body() = std::move(value);
    response_.prepare_payload();
}

void Response::SetCookie(const std::string& name, const std::string& value, int age)
{
    response_.set(Field::set_cookie, utils::format("%1%=%2%; path=/; Max-Age=%3%", name, value, age));
}

Response CreateErrorResponse(std::string message)
{
    boost::json::object error =
    {
        {"code",    0},
        {"message", std::move(message)}
    };

    Response response;
    response.SetStatus(Response::Status::internal_server_error);
    response.SetHeader(Response::Field::content_type, "application/javascript");
    response.SetBody(boost::json::serialize(error));

    return response;
}

}
