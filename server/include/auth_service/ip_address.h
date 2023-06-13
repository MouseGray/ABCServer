#pragma once

#include <string>
#include <boost/hana/define_struct.hpp>
namespace abcserver
{

class IPAddress
{
public:
    static constexpr auto ip_address_size = 4;

    IPAddress();

    IPAddress(const std::string& str);

    bool operator==(const IPAddress& ip_address) const noexcept;

    bool operator!=(const IPAddress& ip_address) const noexcept;

    std::string ToString() const;
private:
    unsigned char data_[ip_address_size];
};

}
