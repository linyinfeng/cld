#include "tcp_stream.h"
#include "wrapper.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>

namespace cld::transport {

TcpStream::TcpStream(const AddressInfo &address)
{
    connect(address);
}

TcpStream::~TcpStream()
{
    close();
}

void TcpStream::connect(const AddressInfo &address)
{
    for (const struct addrinfo &ai : address) {
        try {
            fd = wrapper::Socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
            int flags = fcntl(fd, F_GETFL);
            if (flags == -1)
                throw std::system_error(errno, std::system_category());
            if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
                throw std::system_error(errno, std::system_category());
            if (::connect(fd, ai.ai_addr, ai.ai_addrlen) != 0 && errno != EINPROGRESS) {
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
    if (result != 0) {
        return false;
    }
    return true;
}

bool TcpStream::closed() const
{
    return fd == -1;
}

std::size_t TcpStream::read(std::byte *buf, std::size_t size) {
    if (fd == -1) throw std::runtime_error("Read from closed stream");
    std::size_t left = size;
    while (left > 0) {
        ssize_t read_count;
        if ((read_count = ::read(fd, buf, left)) < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                throw std::system_error(errno, std::system_category());
            }
        }
        else if (read_count == 0) {
            break;
        }
        left -= read_count;
        buf += read_count;
    }
    return size - left;
}

std::size_t TcpStream::write(const std::byte *buf, std::size_t size) {
    if (fd == -1) throw std::runtime_error("Write to closed stream");
    std::size_t left = size;
    const std::byte *p = buf;
    while (left > 0) {
        ssize_t written;
        if ((written = ::write(fd, p, left)) < 0) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                throw std::system_error(errno, std::system_category());
            }
        }
        left -= written;
        p += written;
    }
    return static_cast<std::size_t>(p - buf);
}

void TcpStream::shutdown(int how) {
    if (::shutdown(fd, how) != 0) {
        if (errno != ENOTCONN) {
            throw std::system_error(errno, std::system_category());
        }
    }
}

} // namespace cld::transport
