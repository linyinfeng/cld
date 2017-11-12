#ifndef CLD_WORKER_H_INCLUDED
#define CLD_WORKER_H_INCLUDED

#include <memory>
#include "stream.h"
#include "http.h"
#include "address_info.h"

namespace cld {

class Worker {
public:
    Worker(int epoll_fd, const AddressInfo &address, const std::string scheme,
           const http::Request &request, int file, off_t file_offset);

    int getFileDescriptor() { return stream->getFileDescriptor(); }

    enum class State {
        Connecting, Sending, Receiving, ReceivingBody, Stopped
    };
    State getState() { return state; }

    void process(uint32_t events);

private:
    State state;
    int epoll_fd;

    std::shared_ptr<transport::Stream> stream;

    std::vector<std::byte> request_data;
    std::size_t request_data_sent;

    std::vector<std::byte> response_data;
    char last_char;
    std::shared_ptr<http::Buffer<>> buffer;

    int file;
    off_t file_offset;
};

} // namespace cld

#endif //CLD_WORKER_H_INCLUDED
