#ifndef CLD_TCP_STREAM_H_INCLUDED
#define CLD_TCP_STREAM_H_INCLUDED

#include <cstddef>
#include "address_info.h"
#include "buffer.h"
#include "stream.h"

namespace cld::transport {

class TcpStream : public Stream {
public:
    TcpStream() : fd(-1) { }
    TcpStream(const AddressInfo &address, bool blocking);
    ~TcpStream() override;

    void connect(const AddressInfo &address, bool blocking) override;
    bool continueConnect() override;
    void close() override;
    void shutdown(int how) override;

    bool opened() const override;
    bool closed() const override;

    std::size_t read(std::byte *buf, std::size_t size) override;
    std::size_t write(const std::byte *buf, std::size_t size) override;

    int getFileDescriptor() override { return fd; }

private:
    int fd;
};

} // namespace cld::transport

#endif
