#pragma once

#include <cstring>
#include <ostream>

class string_view
{
public:
    string_view() = default;

    string_view(const char* str) :
        data_{ str }, size_{ std::strlen(str) }
    {

    }

    const char* data() const noexcept
    {
        return data_;
    }

    size_t size() const noexcept
    {
        return size_;
    }
private:
    const char* data_;
    size_t size_;
};

inline std::ostream& operator << (std::ostream& stream, string_view view)
{
    stream << view.data();
    return stream;
}
