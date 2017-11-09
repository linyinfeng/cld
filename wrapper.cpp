#include "wrapper.h"

namespace cld {

namespace wrapper {

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

std::size_t Read(int fd, void *buf, std::size_t count) {
    ssize_t res = read(fd, buf, count);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    return (std::size_t)res;
}

std::size_t Write(int fd, const void *buf, std::size_t count) {
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

std::size_t ReadN(int fd, char *buf, std::size_t count) {
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

std::size_t WriteN(int fd, const char *buf, std::size_t count) {
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
    return count;
}

void Close(int fd) {
    if (close(fd) == -1)
        throw std::system_error(errno, std::system_category());
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

void *GetAddress(const struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return static_cast<void *>(&((struct sockaddr_in *)sa)->sin_addr);
    }
    else if (sa->sa_family == AF_INET6) {
        return static_cast<void *>(&((struct sockaddr_in6 *)sa)->sin6_addr);
    }
    else {
        throw std::runtime_error("Unknow address family");
    }
}
in_port_t GetPort(const struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return ((struct sockaddr_in *)sa)->sin_port;
    }
    else if (sa->sa_family == AF_INET6) {
        return ((struct sockaddr_in6 *)sa)->sin6_port;
    }
    else {
        throw std::runtime_error("Unknow address family");
    }
}

} // namespace cld

} // namespace wrapper