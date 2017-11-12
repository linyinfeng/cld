#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include "address_info.h"
#include "buffer.h"
#include "stream.h"
#include "http.h"
#include "cld.h"
#include "wrapper.h"
#include "worker.h"

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
    http::Request request("GET", initial_url, options);
    request.debugInfo(std::cout);
    int epoll_fd = wrapper::EpollCreate();

    int file = wrapper::Open("temp", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    Worker worker(epoll_fd, address_info, initial_url.getScheme(), request, file, 0);

    struct epoll_event events[10];
    while (true) {
        int count = wrapper::EpollWait(epoll_fd, events, sizeof events / sizeof events[0], 1000);
        for (int i = 0; i < count; ++i) {
            static_cast<Worker *>(events[i].data.ptr)->process(events[i].events);
        }
        if (worker.getState() == Worker::State::Stopped)
            break;
    }

    std::cout << "[Debug] Finished" << std::endl;
}

} // namespace cld
