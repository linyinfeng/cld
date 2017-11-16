#ifndef CLD_WORKER_H_INCLUDED
#define CLD_WORKER_H_INCLUDED

#include <memory>
#include "stream.h"
#include "http.h"
#include "address_info.h"

namespace cld {

static constexpr std::size_t kBufferSize = 1024 * 1024;

class Worker {
public:
    Worker(int epoll_fd, const AddressInfo &address, const std::string &scheme,
           const http::Request &request, int file, off_t file_offset);

    int getFileDescriptor() { return stream->getFileDescriptor(); }
    off_t getOffset() const { return file_offset; }

    /* Worker in state Receiving never enter read data in ReceivingBody state */
    enum class State {
        Connecting, Sending, Receiving, ReceivingBody, Stopped
    };
    State getState() const { return state; }

    std::size_t process(uint32_t events);

    void forceStop();

private:
    State state;
    int epoll_fd;

    std::shared_ptr<transport::Stream> stream;

    std::vector<std::byte> request_data;
    std::size_t request_data_sent;

    std::vector<std::byte> response_data;
    char last_char;
    std::shared_ptr<http::Buffer<kBufferSize>> buffer;

    int file;
    off_t file_offset;
};

} // namespace cld

#endif //CLD_WORKER_H_INCLUDED
