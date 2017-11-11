#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include "address_info.h"
#include "buffer.h"
#include "stream.h"
#include "tcp_stream.h"
#include "http.h"
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
    http::Request request("HEAD", initial_url, options);
    request.debugInfo(std::cout);
    transport::TcpStream stream(address_info);
    http::Write(request, stream);
    http::Response response;
    http::Read(response, stream);
    response.debugInfo(std::cout);

    std::ofstream file("temp");

    std::size_t length = 0;
    if (std::istringstream(response["Content-Length"]) >> length) {
        transport::TcpStream stream(address_info);
        request.setMethod("GET");
        Write(request, stream);
        Read(response, stream);
        response.debugInfo(std::cout);
        SessionBuffer<std::byte> buffer(length);
        while (stream.read(buffer) != 0) {
            std::cout << "Download: " << buffer.end() - buffer.begin() << std::endl;
            for (std::byte b : buffer) {
                file << static_cast<char>(b);
            }
        }
    }

    std::cout << "[Debug] Finished" << std::endl;
}

} // namespace cld
