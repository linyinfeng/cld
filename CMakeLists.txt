cmake_minimum_required(VERSION 3.0)
project(Cld)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-g -Wall -Werror -Wextra -Wno-unused-parameter")

add_executable(
    cld
    address_info.cpp
    address_info.h
    buffer.h
    cld.cpp
    cld.h
    http.h
    http_buffer.h
    http_header.cpp
    http_header.h
    http_request.cpp
    http_request.h
    http_response.cpp
    http_response.h
    length_controller.cpp
    length_controller.h
    main.cpp
    options.cpp
    options.h
    ssl_stream.cpp
    ssl_stream.h
    stream.cpp
    stream.h
    tcp_stream.cpp
    tcp_stream.h
    url.cpp
    url.h
    version.cpp
    version.h
    worker.cpp
    worker.h
    wrapper.cpp
    wrapper.h
    )


find_package(OpenSSL REQUIRED)
target_link_libraries(cld ${OPENSSL_LIBRARIES})
