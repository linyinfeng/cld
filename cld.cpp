#include <iostream>
#include <memory>
#include <sstream>
#include <regex>
#include <fstream>
#include <list>
#include <tuple>
#include <map>
#include "address_info.h"
#include "buffer.h"
#include "stream.h"
#include "http.h"
#include "cld.h"
#include "wrapper.h"
#include "worker.h"
#include "tcp_stream.h"
#include "length_controller.h"

namespace cld {

void PrintHelp() {
    std::cout <<
              "Usage: cld [options] url\n"
                      "--num-connections=x -n x Specify maximum number of connections\n"
                      "--output=x          -o x Specify output file\n"
                      "--user-agent=x      -U x Specify user-agent\n"
                      "--header=x          -H x Add extra header\n"
                      "--help              -h   Show this message\n"
              << std::endl;
}

void Cld(const Options &options, const Url &initial_url) {
    // For test
    AddressInfo address_info(initial_url);
    address_info.debugInfo(std::cout);
    http::Request request("GET", initial_url, options);
    request.debugInfo(std::cout);
    transport::TcpStream stream(address_info, true);
    http::WriteRequest(request, stream);
    stream.shutdown(SHUT_WR);
    auto response = http::ReadToResponse(stream);
    response.debugInfo(std::cout);
    stream.shutdown(SHUT_RD);
    stream.close();

    int file = wrapper::Open("temp", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    if (response["Content-Length"].empty() ||
        response["Accept-Ranges"].empty() ||
        response["Accept-Ranges"] == "none") {
        std::cout << "[Debug] Server report range download unsupported" << std::endl;
        SingleConnectionDownload(address_info, initial_url.getScheme(), request, file);
    } else {
        std::size_t file_size;
        if (std::istringstream(response["Content-Length"]) >> file_size) {
            MultiConnectionDownload(address_info, initial_url.getScheme(),
                                    request, file, file_size, options.getConnectionsNumber());
        } else {
            SingleConnectionDownload(address_info, initial_url.getScheme(), request, file);
        }
    }
    std::cout << "[Debug] Finished" << std::endl;
}

void SingleConnectionDownload(const AddressInfo &address, const std::string &scheme,
                              const http::Request &request, int file) {
    int epoll_fd = wrapper::EpollCreate();
    Worker worker(epoll_fd, address, scheme, request, file, 0);
    auto events = new struct epoll_event[1];
    while (true) {
        int count = wrapper::EpollWait(epoll_fd, events, 1, 1000);
        for (int i = 0; i < count; ++i) {
            static_cast<Worker *>(events[i].data.ptr)->process(events[i].events);
        }
        if (worker.getState() == Worker::State::Stopped) {
            break;
        }
    }
}

void MultiConnectionDownload(const AddressInfo &address, const std::string &scheme,
                             const http::Request &request, int file, std::size_t size, int connection_number) {
    int epoll_fd = wrapper::EpollCreate();
    http::Request req = request;
    std::vector<std::shared_ptr<Worker>> workers(connection_number, nullptr);

    LengthController controller(size, 4096);

    auto events = new struct epoll_event[1];
    do {
        for (std::shared_ptr<Worker> &worker : workers) {
            if (worker.get() != nullptr && worker->getState() == Worker::State::Stopped) {
                // clean
                controller.workerStopped(worker.get());
                worker.reset();
            }
            if (worker.get() == nullptr) {
                off_t begin, end;
                std::tie(begin, end) = controller.next();
                if (end - begin != 0) {
                    req["Range"] = "bytes=" + std::to_string(begin) + "-" + std::to_string(end);
                    std::cout << "[Debug] Change request range header to "
                              << "\"Range\" " << ": \"" + req["Range"] << "\"" << std::endl;
                    worker = std::make_shared<Worker>(epoll_fd, address, scheme, req, file, begin);
                    controller.add(worker.get());
                    controller.debugInfo(std::cout);
                }
            }
        }
        int count = wrapper::EpollWait(epoll_fd, events, 1, 1000);
        for (int i = 0; i < count; ++i) {
            auto worker = static_cast<Worker *>(events[i].data.ptr);
            bool update_controller = false;
            if (worker->getState() == Worker::State::ReceivingBody)
                update_controller = true;
            try {
                worker->process(events[i].events);
            } catch (std::exception &e) {
                worker->forceStop();
                controller.workerStopped(worker);
                std::cout << "[Debug] Worker " << worker << " force stopped because \"" << e.what() << "\"" << std::endl;
            }
            if (update_controller) {
                // if should stop, afterRead return false
                if (!controller.afterRead(worker)) {
                    if (worker->getState() != Worker::State::Stopped) {
                        worker->forceStop();
                        std::cout << "[Debug] Worker " << worker << " force stopped" << std::endl;
                        // Because the structure of the loop, worker can be delete immediately after while true
                    } else {
                        std::cout << "[Debug] Worker " << worker << " stopped" << std::endl;
                    }
                }
            }
        }
        if (controller.finished()) {
            break;
        }
    } while (true);

    wrapper::Close(epoll_fd);
}

} // namespace cld
