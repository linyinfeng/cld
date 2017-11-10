#include "tcp_stream.h"
#include "wrapper.h"

#include <stdexcept>

namespace cld {

namespace transport {

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

void TcpStream::close()
{
    if (fd != -1) {
        wrapper::Close(fd);
        fd = -1;
    }
}

bool TcpStream::opened() const
{
    return fd != -1;
}

bool TcpStream::closed() const
{
    return fd == -1;
}

void TcpStream::read(Buffer<std::byte> &buf) {
    if (fd == -1) throw std::runtime_error("Read from closed stream");
    buf.setValid(0);
    std::size_t read_count = wrapper::Read(fd, buf.data(), buf.size());
    buf.setValid(read_count);
}

void TcpStream::readFill(Buffer<std::byte> &buf) {
    if (fd == -1) throw std::runtime_error("Read from closed stream");
    buf.setValid(0);
    std::size_t read_count = wrapper::ReadN(fd, buf.data(), buf.size());
    buf.setValid(read_count);
}

void TcpStream::write(std::byte *buf, std::size_t size) {
    if (fd == -1) throw std::runtime_error("Write to closed stream");
    wrapper::WriteN(fd, buf, size);
}

} // namespace transport

} // namespace cld
