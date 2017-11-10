#ifndef CLD_STREAM_H_INCLUDED
#define CLD_STREAM_H_INCLUDED

#include <cstddef>
#include "address_info.h"
#include "buffer.h"

namespace cld {

namespace transport {

// A network TcpTcpStream with a seperated buffer
class Stream {
public:
    Stream() { }
    virtual ~Stream() { };

    virtual void read(Buffer<std::byte> &buf) = 0;
    virtual void readFill(Buffer<std::byte> &buf) = 0;
    virtual void write(std::byte *buf, std::size_t size) = 0;

    virtual std::string readLine();

    virtual void connect(const AddressInfo &address) = 0;
    virtual void close() = 0;

    virtual bool opened() const = 0;
    virtual bool closed() const = 0;
};

} // namespace transport

} // namespace cld

#endif
