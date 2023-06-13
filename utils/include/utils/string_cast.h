#pragma once

#include <string>

namespace string_cast
{

template< typename Ty >
std::string ToString(const Ty& value);

template< typename Ty >
Ty FromString(const std::string& value);

}
