#include <error.h>

#include <algorithm>
#include <assert.h>
#include <cstring>
#include <new>

namespace abcserver
{

Error::Error(const Error& error) noexcept :
    type_{error.type_}
{
    if(type_ == ErrorType::UserError)
    {
        message_ = CopyMessage(error.message_);

        if(message_ == nullptr)
        {
            type_ = ErrorType::FatalError;
            message_ = "Bad alloc";
        }
    }
    else
    {
        message_ = error.message_;
    }
}

Error::Error(Error&& error) noexcept :
    type_{error.type_}, message_{error.message_}
{
    error.type_ = ErrorType::NoError;
    error.message_ = nullptr;
}

Error& Error::operator=(const Error& error) noexcept
{
    if(this == &error)
        return *this;

    if(type_ == ErrorType::UserError)
    {
        delete[] message_;
    }

    type_ = error.type_;

    if(type_ == ErrorType::UserError)
    {
        message_ = CopyMessage(error.message_);

        if(message_ == nullptr)
        {
            type_ = ErrorType::FatalError;
            message_ = "Bad alloc";
        }
    }
    else
    {
        message_ = error.message_;
    }

    return *this;
}

Error& Error::operator=(Error&& error) noexcept
{
    if(this == &error)
        return *this;

    std::swap(type_, error.type_);
    std::swap(message_, error.message_);

    return *this;
}

Error::~Error()
{
    if(type_ == ErrorType::UserError)
    {
        delete[] message_;
    }
}

Error::operator bool() const noexcept
{
    return type_ != ErrorType::NoError;
}

const char* Error::message() const noexcept
{
    return message_;
}

Error Error::NoError() noexcept
{
    return Error{ErrorType::NoError, nullptr};
}

Error Error::BadAlloc() noexcept
{
    return Error{ErrorType::FatalError, "Bad alloc"};
}

Error Error::FatalError(const char* message) noexcept
{
    return Error{ErrorType::FatalError, message};
}

Error Error::UserError(const char* message) noexcept
{
    return Error{ErrorType::UserError, message};
}

Error Error::UserError(const std::string& message) noexcept
{
    return Error{ErrorType::UserError, message.c_str()};
}

Error::Error(ErrorType type, const char* message) noexcept :
    type_{type}
{
    if(type_ == ErrorType::UserError)
    {
        message_ = CopyMessage(message);

        if(message_ == nullptr)
        {
            type_ = ErrorType::FatalError;
            message_ = "Bad alloc";
        }
    }
    else
    {
        message_ = message;
    }
}

char* Error::CopyMessage(const char* message) noexcept
{
    assert(message != nullptr);

    auto length = std::strlen(message);

    auto* new_message = new (std::nothrow) char[length + 1]{};

    if(new_message == nullptr)
        return nullptr;

    std::copy(message, message + length, new_message);

    return new_message;
}

}
