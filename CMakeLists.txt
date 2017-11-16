cmake_minimum_required(VERSION 3.0)
project(Cld)

if(NOT CMAKE_BUILD_TYPE)
    message("[Message] Set default build type to release")
    set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(
    cld
    address_info.cpp
    cld.cpp
    http_header.cpp
    http_request.cpp
    http_response.cpp
    length_controller.cpp
    main.cpp
    options.cpp
    ssl_stream.cpp
    stream.cpp
    tcp_stream.cpp
    url.cpp
    version.cpp
    worker.cpp
    wrapper.cpp
    )


find_package(OpenSSL REQUIRED)
target_link_libraries(cld ${OPENSSL_LIBRARIES})
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("[Message] Debug mode, Link profiler")
    target_link_libraries(cld profiler)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cld DESTINATION bin)
