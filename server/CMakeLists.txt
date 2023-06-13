set(CONFIG config.ini)

file(GLOB_RECURSE QUERIES LIST_DIRECTORIES false RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/queries/*.sql)

set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(OpenSSL)

find_package(PostgreSQL)

add_compile_definitions(USE_QUERY_GENERATOR)

foreach (QUERY ${QUERIES})
    string(REGEX REPLACE "[.]sql$" ".cxx" QUERY_CXX ${QUERY})
    get_filename_component(QUERY_CXX ${QUERY_CXX} NAME)
    set(QUERIES_CXX ${QUERIES_CXX} "${CMAKE_BINARY_DIR}/queries/${QUERY_CXX}")
endforeach ()

add_custom_command(
    OUTPUT ${QUERIES_CXX} ${CMAKE_BINARY_DIR}/queries/decl.h
    COMMAND FF_SQL ${CMAKE_CURRENT_SOURCE_DIR}/queries ${CMAKE_BINARY_DIR}/queries ${CMAKE_CURRENT_SOURCE_DIR}/src/template
    DEPENDS ${QUERIES}
)

add_executable(server
    src/main.cpp

    include/auth_service.h              src/auth_service.cpp
    include/auth_service/ip_address.h   src/auth_service/ip_address.cpp
    include/auth_service/session.h      src/auth_service/session.cpp
    include/auth_service/token.h        src/auth_service/token.cpp

    include/config_service.h            src/config_service.cpp

    include/database_service.h          src/database_service.cpp

    include/log_service.h               src/log_service.cpp

    include/method_service.h            src/method_service.cpp
    include/method_service/parameters.h src/method_service/parameters.cpp

    include/web_service.h               src/web_service.cpp
    include/web_service/connection.h    src/web_service/connection.cpp
    include/web_service/request.h       src/web_service/request.cpp
    include/web_service/response.h      src/web_service/response.cpp
    include/web_service/ws_connection.h src/web_service/ws_connection.cpp

    include/error.h                     src/error.cpp

    include/dynamic_data.h              src/dynamic_data.cpp
    ${CONFIG}
    ${QUERIES}
    ${QUERIES_CXX}
    ${CMAKE_BINARY_DIR}/queries/decl.h
)

target_include_directories(server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/utils/include
    ${CMAKE_SOURCE_DIR}/third-party/boost)

target_include_directories(server PUBLIC
    ${CMAKE_BINARY_DIR}/queries)

target_link_libraries(server OpenSSL::SSL PostgreSQL::PostgreSQL)

target_link_libraries(server ${PLATFORM_LIBS} utils)

configure_file( ${CONFIG} ${CONFIG} COPYONLY)

#foreach(file_sql ${QUERIES})
#    configure_file( ${file_sql} ${file_sql} COPYONLY)
#endforeach()



