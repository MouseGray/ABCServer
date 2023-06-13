#pragma once

#include <string>

namespace utils {

std::pair< std::string, std::string > split( const std::string& str, char delimiter );

std::string ltrim( std::string str );

std::string rtrim( std::string str );

std::string trim( std::string str );

}
