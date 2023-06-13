#pragma once

#include <boost/format.hpp>

namespace abcserver {

namespace details {

inline void bind_format(boost::format&) {}

template<typename Arg, typename ...Args>
void bind_format(boost::format& format, Arg&& arg, Args&&... args)
{
    format % arg;
    bind_format(format, std::forward<Args>(args)...);
}

}

namespace utils {

template<typename ...Args>
std::string format(const std::string& str, Args&&... args)
{
    boost::format fmt{str};
    details::bind_format(fmt, std::forward<Args>(args)...);
    return fmt.str();
}

}

}
