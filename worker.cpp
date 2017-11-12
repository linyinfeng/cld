#include "worker.h"
#include <sys/epoll.h>
#include "wrapper.h"

namespace cld {

Worker::Worker(int epoll_fd, const AddressInfo &address, const std::string scheme,
               const http::Request &request, int file, off_t file_offset)
    : epoll_fd(epoll_fd), request_data_sent(0), last_char('\0'),  file(file), file_offset(file_offset) {
    state = State::Connecting;
    request_data = request.request();

    stream = transport::CreateStream(scheme, address);

    struct epoll_event event{};
    event.events = EPOLLOUT | EPOLLERR;
    event.data.ptr = reinterpret_cast<void *>(this);
    wrapper::EpollControl(epoll_fd, EPOLL_CTL_ADD, stream->getFileDescriptor(), &event);
}

void Worker::process(uint32_t event) {
    if (state == State::Connecting) {
        if (event & EPOLLOUT) {
            if (stream->opened()) {
                state = State::Sending;
            } else {
                state = State::Stopped;
            }
        }
    }
    if (state == State::Sending) {
        if (event & EPOLLOUT) {
            std::size_t res = stream->write(request_data.data() + request_data_sent,
                                           request_data.size() - request_data_sent);
            request_data_sent += res;
            if (request_data_sent == request_data.size()) {
                // finish sending
                stream->shutdown(SHUT_WR);

                struct epoll_event new_event{};
                new_event.events = EPOLLIN | EPOLLERR;
                new_event.data.ptr = reinterpret_cast<void *>(this);
                wrapper::EpollControl(epoll_fd, EPOLL_CTL_MOD, stream->getFileDescriptor(), &new_event);
                state = State::Receiving;
            }
        }
    }
    bool recieved_once = false;
    if (state == State::Receiving) {
        if (event & EPOLLIN) {
            char c; auto p = reinterpret_cast<std::byte *>(&c);
            while (stream->read(p, 1) == 1) {
                response_data.push_back(*p);
                if (last_char == '\n' && c == '\r') {
                    if (stream->read(p, 1) == 1 && c == '\n') {
                        // read \n
                        response_data.push_back(*p);
                        http::Response response(response_data);
                        buffer = http::CreateBuffer<>(response);
                        state = State::ReceivingBody;
                        recieved_once = true;
                        break;
                    } else {
                        throw std::runtime_error("No \\n follows \\r in stream");
                    }
                }
                last_char = c;
            }
        }
    }
    if (state == State::ReceivingBody && !recieved_once) {
        if (event & EPOLLIN) {
            if (buffer->read(*stream)) {
                wrapper::LSeek(file, file_offset, SEEK_SET);
                file_offset += wrapper::Write(file, buffer->data().data(), buffer->validCount());
            } else {
                stream->shutdown(SHUT_RD);
                state = State::Stopped;
            }
        }
    }
    if (state == State::Stopped) {
        wrapper::EpollControl(epoll_fd, EPOLL_CTL_DEL, stream->getFileDescriptor(), nullptr);
        stream->close();
        buffer.reset();
        stream.reset();

    }
}

} // namespace cld
