#include <web_service/request.h>

#include <utils.h>

namespace abcserver
{

Request::Request(InternalRequestType request) :
    request_{std::move(request)}
{

}

Request::InternalRequestType& Request::InternalRequest() noexcept
{
    return request_;
}

Request::Verb Request::Method() const
{
    return request_.method();
}

std::string Request::Target() const
{
    return request_.target().to_string();
}

unsigned int Request::Version() const
{
    return request_.version();
}

std::string Request::Header(const std::string& name) const
{
    auto it = request_.find(name);

    if(it == std::end(request_))
        return {};

    return it->value().to_string();
}

std::string Request::Header(Field name) const
{
    auto it = request_.find(name);

    if(it == std::end(request_))
        return {};

    return it->value().to_string();
}

std::string Request::Cookie(const std::string& name) const
{
    auto it = request_.find(boost::beast::http::field::cookie);

    if(it == std::end(request_))
        return {};

    std::string raw_cookie = it->value().to_string();

    std::vector<std::pair<std::string, std::string>> cookies;

    utils::cookie(std::begin(raw_cookie), std::end(raw_cookie), std::back_inserter(cookies));

    auto cookie = std::find_if(std::begin(cookies), std::end(cookies), [name](const std::pair<std::string, std::string>& p) {
        return p.first == name;
    });

    if(cookie == std::end(cookies))
        return {};

    return cookie->second;
}

std::string Request::XMethod() const
{
    auto it = request_.find("X-Method");

    if(it == std::end(request_))
        return {};

    return it->value().to_string();
}

const std::string& Request::Body() const
{
    return request_.body();
}

}
