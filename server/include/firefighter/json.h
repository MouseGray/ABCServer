#pragma once

#include <boost/json.hpp>

namespace abcserver::json
{

using namespace boost::json;

template<typename Ty>
std::string get_as_string(const boost::json::object& json, const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr)
        throw std::invalid_argument{"argument not found '" + name + "'"};

    if constexpr(std::is_same_v<Ty, bool>)
    {
        const auto* value = field->if_bool();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return *value ? "True" : "False";
    }
    else if constexpr(std::is_same_v<Ty, int>)
    {
        const auto* value = field->if_int64();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return std::to_string(*value);
    }
    else if constexpr(std::is_same_v<Ty, double>)
    {
        const auto* value = field->if_double();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return std::to_string(*value);
    }
    else if constexpr(std::is_same_v<Ty, std::string>)
    {
        const auto* value = field->if_string();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return std::string{"'"} + value->c_str() + "'";
    }
    return "null";
}

template<typename Ty>
std::string try_get_as_string(const boost::json::object& json, const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr)
        return "null";

    if constexpr(std::is_same_v<Ty, bool>)
    {
        const auto* value = field->if_bool();

        if(value == nullptr)
            return "null";

        return *value ? "True" : "False";
    }
    else if constexpr(std::is_same_v<Ty, int>)
    {
        const auto* value = field->if_int64();

        if(value == nullptr)
            return "null";

        return std::to_string(*value);
    }
    else if constexpr(std::is_same_v<Ty, double>)
    {
        const auto* value = field->if_double();

        if(value == nullptr)
            return "null";

        return std::to_string(*value);
    }
    else if constexpr(std::is_same_v<Ty, std::string>)
    {
        const auto* value = field->if_string();

        if(value == nullptr)
            return "null";

        return std::string{"'"} + value->c_str() + "'";
    }
    return "null";
}

template<typename Ty>
std::string get_as_string(const boost::json::object& json, const std::string& name, const std::string& default_)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr)
        return default_;

    if(field->is_null())
        return default_;

    if constexpr(std::is_same_v<Ty, bool>)
    {
        const auto* value = field->if_bool();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return *value ? "True" : "False";
    }
    else if(std::is_same_v<Ty, int>)
    {
        const auto* value = field->if_int64();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return std::to_string(*value);
    }
    else if(std::is_same_v<Ty, double>)
    {
        const auto* value = field->if_double();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return std::to_string(*value);
    }
    else if(std::is_same_v<Ty, std::string>)
    {
        const auto* value = field->if_string();

        if(value == nullptr)
            throw std::invalid_argument{"invalid argument type '" + name + "'"};

        return std::string{"'"} + value->c_str() + "'";
    }

    return "null";
}

}
