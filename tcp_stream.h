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
    TcpStream(const AddressInfo &address);
    virtual ~TcpStream();

    virtual void connect(const AddressInfo &address) override;
    virtual void close() override;

    virtual bool opened() const override;
    virtual bool closed() const override;

protected:
    virtual std::size_t _read(std::byte *buf, std::size_t size) override;
    virtual void _write(const std::byte *buf, std::size_t size) override;

private:
    int fd;
};

} // namespace cld::transport

#endif
