#pragma once

#include <cstdint>
#include <string>

namespace abcserver
{

class Error
{
    enum class ErrorType : std::uint8_t
    {
        NoError,
        UserError,
        FatalError
    };
public:
    Error(const Error& error) noexcept;
    Error(Error&& error) noexcept;

    Error& operator=(const Error& error) noexcept;
    Error& operator=(Error&& error) noexcept;

    ~Error();

    operator bool() const noexcept;

    const char* message() const noexcept;

    static Error NoError() noexcept;

    static Error BadAlloc() noexcept;

    static Error FatalError(const char* message) noexcept;

    static Error UserError(const char* message) noexcept;

    static Error UserError(const std::string& message) noexcept;
private:
    Error(ErrorType type, const char* message) noexcept;

    static char* CopyMessage(const char* message) noexcept;

    ErrorType   type_;
    const char* message_;
};

}
