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

set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

add_executable(
    cld
    ${SRC_DIR}/address_info.cpp
    ${SRC_DIR}/cld.cpp
    ${SRC_DIR}/http_header.cpp
    ${SRC_DIR}/http_request.cpp
    ${SRC_DIR}/http_response.cpp
    ${SRC_DIR}/length_controller.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/options.cpp
    ${SRC_DIR}/ssl_stream.cpp
    ${SRC_DIR}/stream.cpp
    ${SRC_DIR}/tcp_stream.cpp
    ${SRC_DIR}/url.cpp
    ${SRC_DIR}/version.cpp
    ${SRC_DIR}/worker.cpp
    ${SRC_DIR}/wrapper.cpp
    )

target_include_directories(cld PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
find_package(OpenSSL REQUIRED)
target_link_libraries(cld ${OPENSSL_LIBRARIES})
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("[Message] Debug mode, Link profiler")
    target_link_libraries(cld profiler)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cld DESTINATION bin)
