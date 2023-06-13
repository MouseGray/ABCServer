#pragma once

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

namespace abcserver
{

class Response
{
    using InternalResponseType = boost::beast::http::response<boost::beast::http::string_body>;
public:
    using Status = boost::beast::http::status;
    using Field  = boost::beast::http::field;

    Response() = default;

    InternalResponseType& InternalResponse() noexcept;

    void SetStatus(Status status);

    void SetVersion(unsigned int version);

    void SetHeader(const std::string& name, const std::string& value);

    void SetHeader(Field name, const std::string& value);

    void SetBody(std::string value);

    void SetCookie(const std::string& name, const std::string& value, int age);
private:
    InternalResponseType response_;
};

Response CreateErrorResponse(std::string message);

}
