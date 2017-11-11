#include <iostream>
#include <memory>
#include "tcp_stream.h"
#include "address_info.h"
#include "http_request.h"
#include "cld.h"
#include "stream.h"

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
    std::shared_ptr<transport::Stream> s = std::make_shared<transport::TcpStream>(address_info);
    http::Write(request, *s);
    Buffer<std::byte> buffer(4096);
    while (s->read(buffer) != 0) {
        for (std::size_t i = 0; i < buffer.valid(); ++i) {
            std::cout << static_cast<char>(buffer[i]);
        }
    }
    std::cout << "[Debug] Finished" << std::endl;
}

}
