#include <iostream>
#include "address_info.h"
#include "request.h"
#include "cld.h"

namespace cld {

void PrintHelp()
{
    std::cout <<
        "Usage: cld [options] url\n"
        "--num-connections=x -n x Specify maximum number of connections\n"
        "--output=x          -o x Specify output file\n"
        "--user-agent=x      -U x Specify user-agent\n"
        "--header=x          -H x Add extra header\n"
        "--help              -h   Show this message\n"
        << std::endl;
}

void Cld(const Options &options, const Url &initial_url)
{
    // For test
    AddressInfo address_info(initial_url);
    address_info.debugInfo(std::cout);
    http::Request request("Head", initial_url, options);
    request.debugInfo(std::cout);
}

}
