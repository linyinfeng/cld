#ifndef CLD_WRAPPER_H_INCLUDED
#define CLD_WRAPPER_H_INCLUDED

#include <stdexcept>
#include <system_error>
#include <cerrno>
#include <string>
#include <cstddef>

#include <sys/socket.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

namespace cld::wrapper {

/* sys/socket.h */
extern int Socket(int domain, int type, int protocol);
extern void Connect(int socket, const struct sockaddr *address, socklen_t address_len);

/* sys/select.h */
extern int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);
extern int Pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, const struct timespec *timeout,
    const sigset_t *sigmask);

/* sys/epoll.h */
extern int EpollCreate();
extern void EpollControl(int epfd, int op, int fd, struct epoll_event *event);
extern int EpollWait(int epfd, struct epoll_event *events, int maxevents, int timeout);

/* unistd.h */
extern std::size_t Read(int fd, std::byte *buf, std::size_t count);
extern std::size_t Write(int fd, const std::byte *buf, std::size_t count);
extern std::size_t Readline(int fd, char *buf, std::size_t max);
extern std::size_t ReadN(int fd, std::byte *buf, std::size_t count);
extern void WriteN(int fd, const std::byte *buf, std::size_t count);
extern void Close(int fd);

/* fcntl.h */
extern off_t LSeek(int fd, off_t offset, int whence);
extern int Open(const std::string &path, int oflag);
extern int Open(const std::string &path, int oflag, mode_t mode);

/* netdb.h */
extern void GetAddressInfo(const char *node, const char *service,
    const struct addrinfo *hints, struct addrinfo **res);

/* arpa/inet.h */
extern const char *InetNtop(int af, const void *src, char *dst, socklen_t size);
extern std::string InetNtop(int af, const void *src);
extern void InetPton(int af, const char *src, void *dst);

/* Other */
extern void *GetAddress(const struct sockaddr *sa);
extern in_port_t GetPort(const struct sockaddr *sa);

} // namespace cld::wrapper

#endif
