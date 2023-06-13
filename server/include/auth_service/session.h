#pragma once

#include <chrono>
#include <string>

#include <auth_service/ip_address.h>
#include <auth_service/token.h>

namespace abcserver
{

class Session
{
public:
    using UserIDType = std::size_t;
    using TimeType   = std::chrono::time_point<std::chrono::system_clock>;

    static constexpr auto invalid_user_id = 0;
    static constexpr auto max_deadline = 30*60;

    Session(UserIDType id, class IPAddress ip_address, class Token token);

    bool IsOverdue() const noexcept;

    void Extend() noexcept;

    const UserIDType UserID() const noexcept;

    const class IPAddress& IPAddress() const noexcept;

    const class Token& Token() const noexcept;
private:
    UserIDType        id_;
    TimeType          deadline_;
    class IPAddress   ip_address_;
    class Token       token_;
};

}
