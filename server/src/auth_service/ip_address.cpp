#include <auth_service/ip_address.h>

#include <stdexcept>

#include <format.h>

namespace abcserver
{

IPAddress::IPAddress() :
    data_{}
{

}

IPAddress::IPAddress(const std::string& str)
{
    const char* first = str.c_str();
    char*       last  = nullptr;

    for(std::size_t i = 0; i < ip_address_size; ++i)
    {
        auto value = std::strtol(first, &last, 10);

        if(value < 0 || 255 < value)
            throw std::invalid_argument{"Invalid ip address '" + str + "'"};

        data_[i] = value;

        if((i + 1) != ip_address_size && *last != '.')
            throw std::invalid_argument{"Invalid ip address '" + str + "'"};

        first = last + 1;
    }
}

bool IPAddress::operator==(const IPAddress& ip_address) const noexcept
{
    return !std::memcmp(data_, ip_address.data_, ip_address_size);
}

bool IPAddress::operator!=(const IPAddress& ip_address) const noexcept
{
    return !(*this == ip_address);
}

std::string IPAddress::ToString() const
{
    return utils::format("%1%.%2%.%3%.%4%", data_[0], data_[1], data_[2], data_[3]);
}

}
