#include <config_service.h>

#include <fstream>

#include <ustring.h>

namespace abcserver
{

ConfigService::ConfigService(const std::string& file)
{
    std::ifstream in{ file };

    if( !in.is_open() )
        throw std::invalid_argument{ "Configuration file not found. Path: " + file };

    Parse(in);
}

void ConfigService::Parse(std::istream& stream)
{
    for(std::string line; std::getline(stream, line);)
    {
        auto r_line = ::utils::split( line, ':' );

        auto key = ::utils::trim( std::move( r_line.first ) );
        auto value = ::utils::trim( std::move( r_line.second ) );

        if( key.empty() || value.empty() )
            continue;

        data_.insert({ std::move( key ), std::move( value ) });
    }
}

template<>
bool ConfigService::GetInternal<std::string>(const std::string& key, std::string& result)
{
    auto it = data_.find(key);

    if(it == std::end( data_ ))
        return false;

    result = it->second;

    return true;
}

template<>
bool ConfigService::GetInternal<int>(const std::string& key, int& result)
{
    auto it = data_.find(key);

    if(it == std::end( data_ ))
        return false;

    result = std::atoi(it->second.c_str());

    return true;
}

}
