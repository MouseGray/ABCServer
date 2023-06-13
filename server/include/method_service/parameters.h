#pragma once

#include <type_traits>

#include <boost/json.hpp>

namespace abcserver
{

namespace method_service
{

template<typename T>
std::string get_as_string(const boost::json::object& json,
                          const std::string& name);

template<>
std::string get_as_string<bool>(const boost::json::object& json,
                                const std::string& name);

template<>
std::string get_as_string<int>(const boost::json::object& json,
                               const std::string& name);

template<>
std::string get_as_string<float>(const boost::json::object& json,
                                 const std::string& name);

template<>
std::string get_as_string<std::string>(const boost::json::object& json,
                                       const std::string& name);

template<typename T>
std::string get_as_string(const boost::json::object& json,
                          const std::string& name, const std::string& def);

template<>
std::string get_as_string<bool>(const boost::json::object& json,
                                const std::string& name,
                                const std::string& def);

template<>
std::string get_as_string<int>(const boost::json::object& json,
                               const std::string& name, const std::string& def);

template<>
std::string get_as_string<float>(const boost::json::object& json,
                                 const std::string& name,
                                 const std::string& def);

template<>
std::string get_as_string<std::string>(const boost::json::object& json,
                                       const std::string& name,
                                       const std::string& def);

template<typename T>
std::string try_get_as_string(const boost::json::object& json,
                              const std::string& name);

template<>
std::string try_get_as_string<bool>(const boost::json::object& json,
                                    const std::string& name);

template<>
std::string try_get_as_string<int>(const boost::json::object& json,
                                   const std::string& name);

template<>
std::string try_get_as_string<float>(const boost::json::object& json,
                                     const std::string& name);

template<>
std::string try_get_as_string<std::string>(const boost::json::object& json,
                                           const std::string& name);

}

}
