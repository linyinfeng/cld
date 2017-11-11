#ifndef CLD_TCP_STREAM_H_INCLUDED
#define CLD_TCP_STREAM_H_INCLUDED

#include <cstddef>
#include "address_info.h"
#include "buffer.h"
#include "stream.h"

namespace cld {

namespace transport {

// A network TcpTcpStream with a seperated buffer
class TcpStream : public Stream {
public:
    TcpStream() : fd(-1) { }
    TcpStream(const AddressInfo &address);
    virtual ~TcpStream();

    virtual std::size_t read(std::byte *buf, std::size_t size) override;
    virtual void write(const std::byte *buf, std::size_t size) override;

    virtual void connect(const AddressInfo &address) override;
    virtual void close() override;

    virtual bool opened() const override;
    virtual bool closed() const override;

private:
    int fd;
};

} // namespace transport

} // namespace cld

#endif
