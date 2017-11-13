#include "wrapper.h"
#include <cstddef>

namespace cld::wrapper {

int Socket(int domain, int type, int protocol)
{
    int res = socket(domain, type, protocol);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return res;
}

void Connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
    if (connect(socket, address, address_len) != 0)
        throw std::system_error(errno, std::system_category());
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
    int res = select(nfds, readfds, writefds, errorfds, timeout);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return res;
}

int Pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds,
    const struct timespec *timeout, const sigset_t *sigmask)
{
    int res = pselect(nfds, readfds, writefds, errorfds, timeout, sigmask);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return res;
}

int EpollCreate() {
    int res = epoll_create(1024);
    if (res == -1) { // size are ignored
        throw std::system_error(errno, std::system_category());
    }
    return res;
}

void EpollControl(int epfd, int op, int fd, struct epoll_event *event) {
    if (epoll_ctl(epfd, op, fd, event) != 0)
        throw std::system_error(errno, std::system_category());
}

int EpollWait(int epfd, struct epoll_event *events, int maxevents, int timeout) {
    int res = epoll_wait(epfd, events, maxevents, timeout);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return res;
}

std::size_t Read(int fd, std::byte *buf, std::size_t count) {
    ssize_t res = read(fd, buf, count);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return (std::size_t)res;
}

std::size_t Write(int fd, const std::byte *buf, std::size_t count) {
    ssize_t res = write(fd, buf, count);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return (std::size_t)res;
}

std::size_t Readline(int fd, char *buf, std::size_t max) {
    char *p = buf;
    bool finished = false;
    std::size_t i;
    for (i = 1; i < max && !finished; ++i) { // space for '\0'
        char c;
        switch (read(fd, &c, 1)) {
        case 1:
            *p++ = c;
            if (c == '\n') {
                finished = true;
            }
            break;
        case 0:
            *p = '\0';
            return i - 1;
            break;
        default:
            if (errno == EINTR) --i;
            else throw std::system_error(errno, std::system_category());
            break;
        }
    }
    *p = '\0';
    return i;
}

std::size_t ReadN(int fd, std::byte *buf, std::size_t count) {
    std::size_t left = count;
    while (left > 0) {
        ssize_t read_count;
        if ((read_count = read(fd, buf, left)) < 0) {
            if (errno == EINTR)
                continue;
            else
                throw std::system_error(errno, std::system_category());
        }
        else if (read_count == 0) {
            break;
        }
        left -= read_count;
        buf += read_count;
    }
    return count - left;
}

void WriteN(int fd, const std::byte *buf, std::size_t count) {
    std::size_t left = count;
    while (left > 0) {
        ssize_t written;
        if ((written = write(fd, buf, left)) <= 0) {
            // error or EINTRS
            if (written < 0 && errno == EINTR)
                continue;
            else
                throw std::system_error(errno, std::system_category());
        }
        left -= written;
        buf += written;
    }
}

void Close(int fd) {
    if (close(fd) == -1)
        throw std::system_error(errno, std::system_category());
}

off_t LSeek(int fd, off_t offset, int whence) {
    off_t res = lseek(fd, offset, whence);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return res;
}

int Open(const std::string &path, int oflag) {
    int res = open(path.c_str(), oflag);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return res;
}

int Open(const std::string &path, int oflag, mode_t mode) {
    int res = open(path.c_str(), oflag, mode);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return res;
}

void GetAddressInfo(const char *node, const char *service,
    const struct addrinfo *hints, struct addrinfo **res)
{
    int result = getaddrinfo(node, service, hints, res);
    if (result != 0) {
        throw std::runtime_error(
            std::string("getaddrinfo error ") + std::to_string(result) + ": " + gai_strerror(result));
    }
}

const char *InetNtop(int af, const void *src, char *dst, socklen_t size)
{
    const char *res = inet_ntop(af, src, dst, size);
    if (res == nullptr)
        throw std::system_error(errno, std::system_category());
    return res;
}

std::string InetNtop(int af, const void *src)
{
    char buffer[INET6_ADDRSTRLEN];
    inet_ntop(af, src, buffer, sizeof buffer);
    return std::string(buffer);
}

void InetPton(int af, const char *src, void *dst) {
    int res = inet_pton(af, src, dst);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    else if (res == 0)
        throw std::runtime_error("Invalid IP address");
}

void *GetAddress(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return reinterpret_cast<void *>(&(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr));
    }
    else if (sa->sa_family == AF_INET6) {
        return reinterpret_cast<void *>(&(reinterpret_cast<sockaddr_in6 *>(sa)->sin6_addr));
    }
    else {
        throw std::runtime_error("Unknow address family");
    }
}
in_port_t GetPort(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return (reinterpret_cast<sockaddr_in *>(sa))->sin_port;
    }
    else if (sa->sa_family == AF_INET6) {
        return (reinterpret_cast<sockaddr_in6 *>(sa))->sin6_port;
    }
    else {
        throw std::runtime_error("Unknow address family");
    }
}

} // namespace cld::wrapper
