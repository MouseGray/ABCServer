#include <method_service/parameters.h>

namespace abcserver
{

namespace method_service
{

template<>
std::string get_as_string<bool>(const boost::json::object& json,
                                const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr)
        throw std::invalid_argument{"argument not found '" + name + "'"};

    const auto* value = field->if_bool();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return *value ? "True" : "False";
}

template<>
std::string get_as_string<int>(const boost::json::object& json,
                               const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr)
        throw std::invalid_argument{"argument not found '" + name + "'"};

    const auto* value = field->if_int64();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return std::to_string(*value);
}

template<>
std::string get_as_string<float>(const boost::json::object& json,
                                 const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr)
        throw std::invalid_argument{"argument not found '" + name + "'"};

    const auto* value = field->if_double();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return std::to_string(*value);
}

template<>
std::string get_as_string<std::string>(const boost::json::object& json,
                                       const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr)
        throw std::invalid_argument{"argument not found '" + name + "'"};

    const auto* value = field->if_string();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return value->c_str();
}

template<>
std::string get_as_string<bool>(const boost::json::object& json,
                                const std::string& name, const std::string& def)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return def;

    const auto* value = field->if_bool();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return *value ? "True" : "False";
}

template<>
std::string get_as_string<int>(const boost::json::object& json,
                               const std::string& name, const std::string& def)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return def;

    const auto* value = field->if_int64();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return std::to_string(*value);
}

template<>
std::string get_as_string<float>(const boost::json::object& json,
                                 const std::string& name,
                                 const std::string& def)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return def;

    const auto* value = field->if_double();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return std::to_string(*value);
}

template<>
std::string get_as_string<std::string>(const boost::json::object& json,
                                       const std::string& name,
                                       const std::string& def)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return def;

    const auto* value = field->if_string();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return value->c_str();
}

template<>
std::string try_get_as_string<bool>(const boost::json::object& json,
                                    const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return "null";

    const auto* value = field->if_bool();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return *value ? "True" : "False";
}

template<>
std::string try_get_as_string<int>(const boost::json::object& json,
                                   const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return "null";

    const auto* value = field->if_int64();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return std::to_string(*value);
}

template<>
std::string try_get_as_string<float>(const boost::json::object& json,
                                     const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return "null";

    const auto* value = field->if_double();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return std::to_string(*value);
}

template<>
std::string try_get_as_string<std::string>(const boost::json::object& json,
                                           const std::string& name)
{
    const auto* field = json.if_contains(name);

    if(field == nullptr || field->is_null())
        return "null";

    const auto* value = field->if_string();

    if(value == nullptr)
        throw std::invalid_argument{"invalid argument type '" + name + "'"};

    return value->c_str();
}

}

}
