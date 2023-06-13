#pragma once

#include <chrono>
#include <iostream>
#include <thread>

#include <format.h>

namespace abcserver
{

class LogService
{
public:
    template<typename ...Args>
    void Info(const std::string& str, Args ...args)
    {
        auto header = CreateHeader("info");
        std::cout << utils::format(header + str + "\n", args...);
        std::cout.flush();
    }

    template<typename ...Args>
    void Warning(const std::string& str, Args ...args)
    {
        auto header = CreateHeader("warn");
        std::cout << utils::format(header + str + "\n", args...) << std::endl;
        std::cout.flush();
    }

    template<typename ...Args>
    void Error(const std::string& str, Args ...args)
    {
        auto header = CreateHeader("erro");
        std::cout << utils::format(header + str + "\n", args...) << std::endl;
        std::cout.flush();
    }
private:
    static std::string CreateHeader(const std::string& prefix)
    {
        // yyyy-MM-dd hh:mm:ss
        // ---------- --------
        //     10    1    8

        auto now    = std::chrono::system_clock::now();
        auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count();
        auto time   = std::chrono::system_clock::to_time_t(now);

        std::array<char, 20> buffer{};
        std::strftime(buffer.data(), buffer.size(), "%F %T", std::localtime(&time));

        return utils::format("[%s|%s.%03d|%#x] ", prefix, buffer.data(), now_ms % 1000, std::this_thread::get_id());
    }
};

}
