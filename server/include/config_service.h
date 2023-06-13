#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace abcserver
{

class ConfigService
{
public:
    ConfigService(const std::string& file);

    template<typename Ty>
    Ty Get(const std::string& key)
    {
        Ty result;
        if(!GetInternal<Ty>(key, result))
            throw std::invalid_argument{"Parameter " + key + " not found"};
        return result;
    }

    template<typename Ty>
    Ty Get(const std::string& key, Ty default_)
    {
        Ty result;
        if(!GetInternal<Ty>(key, result))
            return default_;
        return result;
    }
private:
    void Parse(std::istream& stream);

    template<typename Ty>
    bool GetInternal(const std::string& key, Ty& result);

    std::unordered_map<std::string, std::string> data_;
};

}
