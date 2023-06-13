#pragma once

#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

namespace abcserver
{

class Request
{
    using InternalRequestType = boost::beast::http::request<boost::beast::http::string_body>;
public:
    using Verb  = boost::beast::http::verb;
    using Field = boost::beast::http::field;

    Request() = default;
    Request(InternalRequestType request);

    InternalRequestType& InternalRequest() noexcept;

    Verb Method() const;

    std::string Target() const;

    unsigned int Version() const;

    std::string Header(const std::string& name) const;

    std::string Header(Field name) const;

    const std::string& Body() const;

    std::string Cookie(const std::string& name) const;

    std::string XMethod() const;
private:
    InternalRequestType request_;
};

}
