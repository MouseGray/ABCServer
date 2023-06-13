#include <auth_service/token.h>

#include <cstring>
#include <stdexcept>

#include <openssl/err.h>
#include <openssl/rand.h>

#include <utils.h>

#define MAX_ERROR_SIZE 256

namespace abcserver
{

Token::Token() :
    data_{}
{

}

Token::Token(const std::string& str)
{
    if(str.size() != token_size)
        throw std::invalid_argument{"Invalid token '" + str + "'"};

    std::copy(std::begin( str ), std::end( str ), data_);
}

bool Token::operator==(const Token& token) const noexcept
{
    return !std::memcmp(data_, token.data_, token_size);
}

bool Token::operator!=(const Token& token) const noexcept
{
    return !(*this == token);
}

bool Token::operator<(const Token& token) const noexcept
{
    return std::memcmp(data_, token.data_, token_size) == -1;
}

bool Token::operator>(const Token& token) const noexcept
{
    return std::memcmp(data_, token.data_, token_size) == 1;
}

bool Token::IsNull() const noexcept
{
    return data_[0] == 0;
}

std::string Token::ToString() const
{
    std::string result;

    std::copy(data_, data_+ token_size, std::back_inserter(result));

    return result;
}

Token Token::GenerateToken()
{
    Token token;

    unsigned char rand_sequence[3 * (token_size / 4)];
    char          error_buffer[MAX_ERROR_SIZE];

    switch(RAND_bytes(rand_sequence, sizeof(rand_sequence))) {
        case -1:
            throw std::invalid_argument{"RAND_bytes() is not supported"};
        case 0:
            ERR_error_string_n(ERR_get_error(), error_buffer, sizeof(error_buffer));
            throw std::invalid_argument{error_buffer};
        default:
            break;
    }

    utils::base64_encode(rand_sequence, rand_sequence + sizeof(rand_sequence), token.data_);

    return token;
}

}
