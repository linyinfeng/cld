#include "stream.h"
#include "wrapper.h"

#include <stdexcept>

namespace cld {

Stream::Stream(const AddressInfo &address)
{
    connect(address);
}

Stream::~Stream()
{
    close();
}

int Stream::fileDescriptor()
{
    if (fd != -1)
        return fd;
    else
        throw std::runtime_error("Access invalid stream file descriptor");
}

void Stream::connect(const AddressInfo &address)
{
    for (const struct addrinfo &ai : address) {
        try {
            fd = wrapper::Socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
            wrapper::Connect(fd, ai.ai_addr, ai.ai_addrlen);
            break;
        }
        catch (std::system_error &e) {
            fd = -1;
            continue;
        }
    }
    if (fd == -1)
        throw std::runtime_error("Failed to connect");
}

void Stream::close()
{
    if (fd != -1) {
        wrapper::Close(fd);
        fd = -1;
    }
}

bool Stream::opened()
{
    return fd != -1;
}

bool Stream::closed()
{
    return fd == -1;
}

}