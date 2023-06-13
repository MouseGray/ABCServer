#pragma once

#include <string>
#include <algorithm>
#include <cstring>
#include <vector>

#include "stringview.h"

#define u_text(...) utils::concat_strings(__VA_ARGS__)

namespace utils {

constexpr char base64[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '_'
};

constexpr char rbase64[] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x3E,0x0, 0x0, 0x0, 0x3F,
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,
    0x3C,0x3D,0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6,
    0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE,
    0xF, 0x10,0x11,0x12,0x13,0x14,0x15,0x16,
    0x17,0x18,0x19,0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,
    0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,
    0x31,0x32,0x33,0x0, 0x0, 0x0, 0x0, 0x0
};

template<typename Iit, typename Oit>
void base64_encode(Iit beg, Iit end, Oit out)
{
    uint32_t char_1, char_2, char_3;
    uint32_t segment = 0;

    while(beg != end)
    {
        switch (std::distance(beg, end))
        {
            case 0:
                return;
            case 1:
                char_1 = *beg; ++beg;

                segment = 0x0 | (char_1 << 16);

                *out = base64[(segment >> 18) & 0x3F]; ++out;
                *out = base64[(segment >> 12) & 0x3F]; ++out;
                *out = '='; ++out;
                *out = '='; ++out;
                break;
            case 2:
                char_1 = *beg; ++beg;
                char_2 = *beg; ++beg;

                segment = 0x0 | (char_1 << 16) | (char_2 << 8);

                *out = base64[(segment >> 18) & 0x3F]; ++out;
                *out = base64[(segment >> 12) & 0x3F]; ++out;
                *out = base64[(segment >> 6 ) & 0x3F]; ++out;
                *out = '='; ++out;
                break;
            default:
                char_1 = *beg; ++beg;
                char_2 = *beg; ++beg;
                char_3 = *beg; ++beg;

                segment = 0x0 | (char_1 << 16) | (char_2 << 8) | (char_3);

                *out = base64[(segment >> 18) & 0x3F]; ++out;
                *out = base64[(segment >> 12) & 0x3F]; ++out;
                *out = base64[(segment >> 6 ) & 0x3F]; ++out;
                *out = base64[(segment >> 0 ) & 0x3F]; ++out;
        }
    }
}

template<typename Iit, typename Oit>
void base64_decode(Iit beg, Iit end, Oit out)
{
    uint32_t char_1, char_2, char_3, char_4;
    uint32_t segment = 0;

    auto length = [](Iit beg, Iit end) {
        if(std::distance(beg, end) < 4) return 0;
        end = beg; std::advance(end, 4);
        auto it = std::find(beg, end, '=');
        return static_cast<int>(std::distance(beg, it));
    };

    while(beg != end)
    {
        switch (length(beg, end))
        {
            case 0: case 1:
                return;
            case 2:
                char_1 = rbase64[static_cast<int>(*beg)]; ++beg;
                char_2 = rbase64[static_cast<int>(*beg)]; ++beg;

                segment = 0x0 | (char_1 << 18) | (char_2 << 12);

                *out = (segment >> 16) & 0xFF; ++out;
                break;
            case 3:
                char_1 = rbase64[static_cast<int>(*beg)]; ++beg;
                char_2 = rbase64[static_cast<int>(*beg)]; ++beg;
                char_3 = rbase64[static_cast<int>(*beg)]; ++beg;

                segment = 0x0 | (char_1 << 18) | (char_2 << 12) | (char_3 << 6);

                *out = (segment >> 16) & 0xFF; ++out;
                *out = (segment >> 8 ) & 0xFF; ++out;
                break;
            default:
                char_1 = rbase64[static_cast<int>(*beg)]; ++beg;
                char_2 = rbase64[static_cast<int>(*beg)]; ++beg;
                char_3 = rbase64[static_cast<int>(*beg)]; ++beg;
                char_4 = rbase64[static_cast<int>(*beg)]; ++beg;

                segment = 0x0 | (char_1 << 18) | (char_2 << 12) | (char_3 << 6) | (char_4);

                *out = (segment >> 16) & 0xFF; ++out;
                *out = (segment >> 8 ) & 0xFF; ++out;
                *out = (segment >> 0 ) & 0xFF; ++out;
        }
    }
}

inline int to_int(const char* begin, const char** end)
{
    int result = 0;
    while(true)
    {
        const char ch = *begin;

        if(!std::isdigit(ch))
            break;

        result = result*10 + (ch - '0');

        ++begin;
    }

    if(end)
        *end = begin;

    return result;
}

template<typename Ty>
inline Ty to_value(const char* value);

template<>
inline int to_value<int>(const char* value) {
    return atoi(value);
}

template<>
inline bool to_value<bool>(const char* value) {
    return value[0] == 't';
}

template<>
inline double to_value<double>(const char* value) {
    return atof(value);
}

template<>
inline std::string to_value<std::string>(const char* value) {
    return { value };
}

template<>
inline std::vector<int> to_value<std::vector<int>>(const char* value)
{
    std::vector<int> result;

    for(++value; *value; ++value)
    {
        const char* end = nullptr;
        auto num = to_int(value, &end);

        if(value == end)
            break;

        value = end;

        result.push_back(num);
    }

    return result;
}


inline std::string to_string(int value) {
    return std::to_string(value);
}

inline std::string to_string(const char* value) {
    return { value };
}

inline std::string to_string(StringView value) {
    return { value.data(), value.size() };
}

inline std::string to_string(std::string value) {
    return value;
}


constexpr size_t digit_count(size_t number) {
    size_t count_ = 0;
    while (number) {
        number /= 10;
        ++count_;
    }
    return count_;
}

std::string current_date_time(const std::string& format);

template<size_t N>
constexpr inline size_t size(const char(&)[N]) {
    return N - 1;
}

template<size_t N>
constexpr inline const char* begin(const char(&arr)[N]) {
    return arr;
}

template<size_t N>
constexpr inline const char* end(const char(&arr)[N]) {
    return arr + N - 1;
}

inline size_t size(const char* arr) {
    return std::strlen(arr);
}

inline size_t size(const std::string& str)
{
    return str.size();
}

constexpr inline const char* begin(const char* arr) {
    return arr;
}

inline const char* end(const char* arr) {
    return arr + std::strlen(arr);
}

inline size_t full_size()
{
    return 0;
}

template<typename Arg, typename ...Args>
inline size_t full_size(Arg arg, Args ...args)
{
    return size(arg) + full_size(args...);
}

inline void append(std::string&)
{

}

template<typename Arg, typename ...Args>
inline void append(std::string& res, Arg arg, Args ...args)
{
    res.append(begin(arg), end(arg));
    append(res, args...);
}

template<typename ...Args>
std::string concat_strings(Args&& ...args) {
    size_t size_ = 1 + full_size(args...);

    std::string result;
    result.reserve(size_);

    append(result, args...);

    return result;
}

size_t stream_size(std::istream &stream);

constexpr uint8_t token[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0
};

inline bool is_cookie_octet(char ch)
{
    return ch == 0x21 ||
            (0x23 <= ch && ch <= 0x2B) || (0x2D <= ch && ch <= 0x3A) ||
            (0x3C <= ch && ch <= 0x5B) || (0x5D <= ch && ch <= 0x7E);
}

inline bool is_token(char ch)
{
    return token[static_cast<int>(ch)];
}

template< typename Iit >
Iit cookie_value_impl(Iit beg, Iit end)
{
    return std::find_if_not(beg, end, is_cookie_octet);
}

template< typename Iit >
Iit cookie_value(Iit beg, Iit end)
{
    if(beg == end)
        return beg;

    if(*beg == '"')
    {
        auto last = cookie_value_impl(std::next(beg), end);

        if(last == end || *last != '"')
            return beg;

        return std::next(last);
    }

    return cookie_value_impl(beg, end);
}

template< typename Iit >
Iit cookie_name(Iit beg, Iit end)
{
    if(beg == end)
        return beg;

    return std::find_if_not(beg, end, is_token);
}

template< typename IIt, typename OIt >
IIt cookie_pair(IIt beg, IIt end, OIt out)
{
    if(beg == end)
        return beg;

    auto name_end = cookie_name(beg, end);

    if(name_end == beg)
        return beg;

    if(*name_end != '=')
        return beg;

    auto value_end = cookie_value(std::next(name_end), end);

    *out++ = std::make_pair< std::string, std::string >( { beg, name_end }, { std::next(name_end), value_end } );

    return value_end;
}

template< typename IIt, typename OIt >
void cookie(IIt beg, IIt end, OIt out)
{
    while(beg != end)
    {
        auto next = cookie_pair(beg, end, out);

        if(next == beg || *next != ';')
            return;

        ++next;

        if(next == beg || *next != ' ')
            return;

        beg = ++next;
    }
}

std::string read_file(const std::string& path);

bool starts_with(const std::string& lhs, const std::string& rhs) noexcept;

bool is_substr(const std::string& lhs, const std::string& rhs) noexcept;

}
