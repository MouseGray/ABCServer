#pragma once

#include <string>

namespace abcserver
{

class Token
{
public:
    static constexpr auto token_size = 40;

    Token();

    Token(const std::string& str);

    bool operator==(const Token& token) const noexcept;

    bool operator!=(const Token& token) const noexcept;

    bool operator<(const Token& token) const noexcept;

    bool operator>(const Token& token) const noexcept;

    bool IsNull() const noexcept;

    std::string ToString() const;

    static Token GenerateToken();
private:
    unsigned char data_[token_size];
};

}
