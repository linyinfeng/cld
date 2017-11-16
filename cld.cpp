#include <iostream>
#include <memory>
#include <sstream>
#include <regex>
#include <fstream>
#include <list>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <algorithm>
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

static const std::string kInfoStyle("\033[1m");
static const std::string kCleanStyle("\033[0m");

static void SingleConnectionDownload(const AddressInfo &address, const std::string &scheme,
                                     const http::Request &request, int file);
static void MultiConnectionDownload(const AddressInfo &address, const std::string &scheme,
                                    const http::Request &request, int file, std::size_t size, int connection_number);
static std::string BytesCountToHumanReadable(std::size_t bytes, bool standard = false);

void Cld(Options &options, Url &url) {
    bool find = false;
    AddressInfo address_info;
    http::Request request;
    http::Response response;

    /* Follow redirects */
    do {
        // if no path have been set
        if (options.getOutputPath().empty()) {
            const std::string &path = url.getPath();
            std::string::size_type pos = path.find_last_of('/');
            std::string new_path;
            if (pos == std::string::npos) {
                new_path = path;
            } else {
                new_path = path.substr(pos + 1, std::string::npos);
            }
            if (!new_path.empty()) {
                options.setOutputPath(new_path);
                std::cout << kInfoStyle << "[Info] Set path to " << options.getOutputPath() << kCleanStyle << std::endl;
            }
        }

        /* Get response */
        address_info = AddressInfo(url);
        address_info.debugInfo(std::cout);
        request = http::Request("HEAD", url, options);
        request.debugInfo(std::cout);
        auto stream = transport::CreateStream(url.getScheme(), address_info, true);
        if (!stream->opened()) {
            throw std::runtime_error("Connect failed");
        }
        http::WriteRequest(request, *stream);
        stream->shutdown(SHUT_WR);
        response = http::ReadToResponse(*stream);
        response.debugInfo(std::cout);
        stream->shutdown(SHUT_RD);
        stream->close();

        /* Analysis response */
        switch (response.getStatus()) {
            case 300: case 301: case 302: case 307:
            {
                std::string location = response["Location"];
                if (location.empty())
                    throw std::runtime_error("No Location given in redirects");
                try {
                    // Absolute
                    url = Url(location);
                } catch (std::exception &e) {
                    // Relative
                    //                 /path                               ?query                                    #fragment
                    std::regex regex(R"regex(\/([a-zA-Z0-9._~!$&'()*+,;=:@%\/-]*)(?:\?([a-zA-Z0-9._~!$&'()*+,;=:@\/?%-]*))?(?:#([a-zA-Z0-9._~!$&'()*+,;=:@\/?%-]*))?)regex");
                    std::smatch matches;
                    if (std::regex_match(location, matches, regex)) {
                        url.setPath(matches[1]);
                        url.setQuery(matches[2]);    // Usually empty
                        url.setFragment(matches[3]); // Usually empty
                    } else {
                        throw std::runtime_error("Invalid Location in redirects: " + location);
                    }
                }
            }
                break;
            case 303: case 304: case 305:
                throw std::runtime_error("Unsupported redirect");
            default:
                if (response.isOk()) {
                    find = true;
                } else {
                    throw std::runtime_error("Failed to get resource: " +
                                             std::to_string(response.getStatus()) + " - " + response.getStatusText());
                }
        }

        /* set cookie */
        if (!response["Set-Cookie"].empty()) {
            options.addExtraHeader("Cookie", response["Set-Cookie"]);
        }
    } while (!find);

    int file;
    if (options.getOutputPath().empty()) {
        char filename[] = "cld_XXXXXX";
        file = mkstemp(filename);
        options.setOutputPath(filename);
    } else {
        file = wrapper::Open(options.getOutputPath(), O_WRONLY | O_CREAT | O_TRUNC, 0600);
    }

    request.setMethod("GET");

    // Select download method
    if (
        //response["Accept-Ranges"].empty() ||
        response["Accept-Ranges"] == "none") {
        std::size_t file_size;
        if (std::istringstream(response["Content-Length"]) >> file_size) {
            std::cout << "[Debug] Server report range download unsupported" << std::endl;
            std::cout << kInfoStyle << "[Info] File size set to " << file_size << kCleanStyle << std::endl;
            MultiConnectionDownload(address_info, url.getScheme(),
                                    request, file, file_size, 1);
        } else {
            std::cout << "[Debug] Server report range download unsupported" << std::endl;
            SingleConnectionDownload(address_info, url.getScheme(), request, file);
        }
    } else {
        std::size_t file_size;
        if (std::istringstream(response["Content-Length"]) >> file_size) {
            std::cout << kInfoStyle << "[Info] File size set to " << file_size << kCleanStyle << std::endl;
            MultiConnectionDownload(address_info, url.getScheme(),
                                    request, file, file_size, options.getConnectionsNumber());
        } else {
            std::cout << "[Debug] Server report range download unsupported" << std::endl;
            SingleConnectionDownload(address_info, url.getScheme(), request, file);
        }
    }

    std::cout << kInfoStyle << "[Info] Finished" << kCleanStyle << std::endl;
    std::cout << kInfoStyle << "[Info] File saved to " + options.getOutputPath() << kCleanStyle << std::endl;
}

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

static void SingleConnectionDownload(const AddressInfo &address, const std::string &scheme,
                              const http::Request &request, int file) {
    auto last_time = std::chrono::system_clock::now();
    std::size_t last_downloaded = 0;

    int epoll_fd = wrapper::EpollCreate();
    Worker worker(epoll_fd, address, scheme, request, file, 0);
    struct epoll_event events[10];
    while (true) {
        int count = wrapper::EpollWait(epoll_fd, events, sizeof events / sizeof *events, 1000);
        for (int i = 0; i < count; ++i) {
            last_downloaded += static_cast<Worker *>(events[i].data.ptr)->process(events[i].events);
        }

        if (worker.getState() == Worker::State::Stopped) {
            break;
        }

        auto time = std::chrono::system_clock::now();
        if ( (time - last_time) >= std::chrono::milliseconds(250)) {
            std::cout << kInfoStyle
                      << "[Info] "
                      << "Download with single connection"
                      << " Speed: "
                      << BytesCountToHumanReadable(
                              static_cast<std::size_t>(
                                      1000 * last_downloaded / std::chrono::duration_cast<std::chrono::milliseconds>(time - last_time).count()
                              )
                      )
                      << "/s"
                      << kCleanStyle
                      << std::endl;
            last_time = time;
            last_downloaded = 0;
        }
    }
}

static void MultiConnectionDownload(const AddressInfo &address, const std::string &scheme,
                             const http::Request &request, int file, std::size_t size, int connection_number) {
    auto last_time = std::chrono::system_clock::now();
    off_t last_remain = size;

    int epoll_fd = wrapper::EpollCreate();
    http::Request req = request;
    std::vector<std::shared_ptr<Worker>> workers(static_cast<size_t>(connection_number));
    LengthController controller(size, 1024 * 1024);

    auto events = new struct epoll_event[connection_number];
    do {
        for (std::shared_ptr<Worker> &worker : workers) {
            if (worker != nullptr && worker->getState() == Worker::State::Stopped) {
                // clean
                controller.workerStopped(worker.get());
                worker.reset();
            }
            if (worker == nullptr) {
                off_t begin, end;
                std::tie(begin, end) = controller.next();
                if (end - begin != 0) {
                    req["Range"] = "bytes=" + std::to_string(begin) + "-" + std::to_string(end);
                    std::cout << "[Debug] Change request range header to "
                              << "\"Range\" " << ": \"" + req["Range"] << "\"" << std::endl;
                    worker = std::make_shared<Worker>(epoll_fd, address, scheme, req, file, begin);
                    controller.add(worker.get());
                    std::cout << kInfoStyle << "[Info] New worker created" << kCleanStyle << std::endl;
                    //controller.debugInfo(std::cout);
                }
            }
        }
        int count = wrapper::EpollWait(epoll_fd, events, connection_number, 1000); // 1 second timeout
        for (int i = 0; i < count; ++i) {
            auto worker = static_cast<Worker *>(events[i].data.ptr);
            bool is_receiving_body = false;
            if (worker->getState() == Worker::State::ReceivingBody)
                is_receiving_body = true;
            try {
                worker->process(events[i].events);
            } catch (std::exception &e) {
                worker->forceStop();
                controller.workerStopped(worker);
                std::cout << "[Debug] Worker " << worker << " force stopped because \"" << e.what() << "\"" << std::endl;
            }
            if (is_receiving_body) {
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

        auto time = std::chrono::system_clock::now();
        if ( (time - last_time) >= std::chrono::milliseconds(250)) {
            off_t remain = controller.remain();
            std::cout << kInfoStyle
                      << "[Info] "
                      << "Connections: " << controller.workerNumber()
                      << " Remain: " << BytesCountToHumanReadable(static_cast<std::size_t>(remain))
                      << " Speed: "
                      << BytesCountToHumanReadable(
                              static_cast<std::size_t>(
                                      1000 * (last_remain - remain) / std::chrono::duration_cast<std::chrono::milliseconds>(time - last_time).count()
                              )
                      )
                      << "/s"
                      << kCleanStyle
                      << std::endl;
            last_time = time;
            last_remain = remain;
        }
        if (controller.finished()) {
            break;
        }
    } while (true);

    delete[] events;
    wrapper::Close(epoll_fd);
}

static std::string BytesCountToHumanReadable(std::size_t bytes, bool standard) {
    std::size_t unit = standard ? 1000 : 1024;
    if (bytes < unit) return std::to_string(bytes) + " B";
    auto exp = static_cast<std::size_t>(std::log(bytes) / std::log(unit));
    if (exp > 7) exp = 6;
    std::string unit_string(standard ? "kMGTPE" : "KMGTPE");
    std::ostringstream ss;
    ss << std::setiosflags(std::ios::fixed) << std::setprecision(2) << (bytes / std::pow(unit, exp))
       << " " << unit_string[exp - 1] << (standard ? "" : "i") << "B";
    return ss.str();
}

} // namespace cld
