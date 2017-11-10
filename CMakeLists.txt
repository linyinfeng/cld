cmake_minimum_required (VERSION 3.0)
project (Cld)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "-g -Wall -Werror -Wextra -Wno-unused-parameter" )

add_executable(
    cld
    address_info.cpp
    cld.cpp
    main.cpp
    options.cpp
    request.cpp
    stream.cpp
    url.cpp
    version.cpp
    wrapper.cpp
)
