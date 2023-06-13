#include <auth_service/session.h>


namespace abcserver
{

Session::Session(UserIDType id, class IPAddress ip_address, class Token token) :
    id_{id}, ip_address_{ip_address}, token_{token}
{
    deadline_ = std::chrono::system_clock::now() + std::chrono::seconds{+Session::max_deadline};
}

bool Session::IsOverdue() const noexcept
{
    return deadline_ < std::chrono::system_clock::now();
}

void Session::Extend() noexcept
{
    deadline_ = std::chrono::system_clock::now() + std::chrono::seconds{+Session::max_deadline};
}

const Session::UserIDType Session::UserID() const noexcept
{
    return id_;
}

const class IPAddress& Session::IPAddress() const noexcept
{
    return ip_address_;
}

const class Token& Session::Token() const noexcept
{
    return token_;
}

}
