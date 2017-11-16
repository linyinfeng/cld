#include "tcp_stream.h"
#include "wrapper.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>

namespace cld::transport {

TcpStream::TcpStream(const AddressInfo &address, bool blocking)
{
    connect(address, blocking);
}

TcpStream::~TcpStream()
{
    close();
}

void TcpStream::connect(const AddressInfo &address, bool blocking)
{
    for (const struct addrinfo &ai : address) {
        try {
            fd = wrapper::Socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
            if (::connect(fd, ai.ai_addr, ai.ai_addrlen) != 0 && errno != EINPROGRESS) {
                throw std::system_error(errno, std::system_category());
            }
            if (!blocking) {
                int flags = fcntl(fd, F_GETFL);
                if (flags == -1)
                    throw std::system_error(errno, std::system_category());
                if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
                    throw std::system_error(errno, std::system_category());
            }
            break;
        }
        catch (std::system_error &e) {
            if (fd != -1) {
                wrapper::Close(fd);
                fd = -1;
            }
            continue;
        }
    }
}

void TcpStream::close()
{
    if (fd != -1) {
        if (::close(fd) == -1)
            throw std::system_error(errno, std::system_category());
        fd = -1;
    }
}

bool TcpStream::opened() const
{
    if (fd == -1) return false;
    int result;
    socklen_t result_len = sizeof(result);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) {
        return false;
    }
    return result == 0;
}

bool TcpStream::closed() const
{
    return !opened();
}

std::size_t TcpStream::read(std::byte *buf, std::size_t size) {
    if (fd == -1) throw std::runtime_error("Read from closed stream");
    while (true) {
        ssize_t read_count;
        if ((read_count = ::read(fd, buf, size)) < 0) {
            if (errno == EINTR) {
                continue; // retry
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                throw StreamException(StreamException::Exception::kAgain);
            } else {
                throw std::system_error(errno, std::system_category());
            }
        }
        return static_cast<std::size_t>(read_count);
    }
}

std::size_t TcpStream::write(const std::byte *buf, std::size_t size) {
    if (fd == -1) throw std::runtime_error("Read from closed stream");
    while (true) {
        ssize_t write_count;
        if ((write_count = ::write(fd, buf, size)) < 0) {
            // Blocking
            if (errno == EINTR) {
                continue; // retry
            // Non-blocking
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                throw StreamException(StreamException::Exception::kAgain);
            } else {
                throw std::system_error(errno, std::system_category());
            }
        }
        return static_cast<std::size_t>(write_count);
    }
}

void TcpStream::shutdown(int how) {
    if (::shutdown(fd, how) != 0) {
        if (errno != ENOTCONN) {
            throw std::system_error(errno, std::system_category());
        }
    }
}

} // namespace cld::transport
