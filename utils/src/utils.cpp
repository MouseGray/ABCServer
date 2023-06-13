#include "utils.h"

#include <iostream>
#include <fstream>
#include <ctime>

namespace utils {

std::string current_date_time(const std::string &format) {
    std::time_t t = std::time(nullptr);

    char buffer[64];
    auto size = std::strftime(buffer, sizeof(buffer), format.c_str(), std::localtime(&t));

    return { buffer, size };
}

size_t stream_size(std::istream &stream)
{
    stream.clear();
    size_t cur = stream.tellg();
    stream.seekg(0, std::ios_base::end);
    size_t size = stream.tellg();
    stream.seekg(cur, std::ios_base::beg);
    stream.clear();
    return size;
}

std::string read_file(const std::string &path)
{
    std::ifstream in{ path };

    if(!in.is_open())
        throw std::invalid_argument{ u_text(u8"Не удалось открыть файл ", path) };

    return std::string{ std::istreambuf_iterator< char >{ in },
        std::istreambuf_iterator< char >{} };
}

bool starts_with(const std::string &lhs, const std::string &rhs) noexcept
{
    auto p_it = std::mismatch(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    return p_it.second == rhs.end();
}

bool is_substr(const std::string &lhs, const std::string &rhs) noexcept
{
    if(rhs.empty())
        return true;

    size_t start = 0;
    char first = rhs.front();

    while (true) {
        start = lhs.find(first, start);

        if(start == std::string::npos)
            return false;

        if(lhs.size() - start < rhs.size())
            return false;

        if(lhs.compare(start, rhs.size(), rhs) == 0)
            return true;

        ++start;
    }

    return false;
}

}
