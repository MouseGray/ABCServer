set(
    HEADERS
    include/utils/stringview.h
    include/utils/format.h
    include/utils/ustring.h
)

set(
    SOURCES
    src/stringview.cpp
    src/ustring.cpp
    src/utils.cpp
    include/utils/string_cast.h src/string_cast.cpp
)

add_library(utils ${SOURCES} ${HEADERS})

target_include_directories(utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${CMAKE_SOURCE_DIR}/third-party/boost
)

