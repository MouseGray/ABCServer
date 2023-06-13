#include "ustring.h"

#include <algorithm>
#include <cctype>
#include <functional>

std::pair< std::string, std::string > utils::split( const std::string& str, char delimiter )
{
    auto delimeter_pos = str.find( delimiter );

    if( delimeter_pos == std::string::npos )
        return { str, {} };

    return { str.substr( 0, delimeter_pos ), str.substr( delimeter_pos + 1 ) };
}

std::string utils::ltrim( std::string str )
{
    auto begin = std::find_if( str.begin(), str.end(),  [](auto&& val){
        return !std::isspace(val); });

    str.erase( str.begin(), begin );

    return str;
}

std::string utils::rtrim(std::string str)
{
    auto end = std::find_if( str.rbegin(), str.rend(), [](auto&& val){
        return !std::isspace(val); });

    str.erase( end.base(), str.end() );

    return str;
}

std::string utils::trim(std::string str)
{
    return rtrim( ltrim( str ) );
}
