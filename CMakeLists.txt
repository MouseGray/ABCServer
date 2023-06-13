cmake_minimum_required(VERSION 3.5)

project(ABCServer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(PLATFORM_LIBS -lwsock32 -lws2_32)
else()
    set(PLATFORM_LIBS)
endif()

add_compile_definitions(PostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql)

add_compile_definitions(CMAKE_GENERATOR_PLATFORM=x86)

add_compile_definitions(BOOST_ALL_NO_LIB=1)

add_subdirectory(server)

add_subdirectory(utils)

#add_subdirectory(third-party/libstudxml)

#add_subdirectory(third-party/xlnt)

#add_subdirectory(third-party/asio)

add_subdirectory(third-party/boost)

enable_testing()
