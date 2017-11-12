#ifndef CLD_STREAM_H_INCLUDED
#define CLD_STREAM_H_INCLUDED

#include <cstddef>
#include <string>
#include <vector>
#include "address_info.h"
#include "buffer.h"

namespace cld::transport {

class Stream;

extern std::shared_ptr<Stream> CreateStream(const std::string &scheme, const AddressInfo &addr);

// A network TcpTcpStream with a seperated buffer
class Stream {
public:
    Stream() { }
    virtual ~Stream() { };

    virtual std::size_t write(const std::byte *buf, std::size_t size) = 0;
    virtual std::size_t read(std::byte *buf, std::size_t count) = 0;

    virtual void connect(const AddressInfo &address) = 0;
    virtual void close() = 0;
    virtual void shutdown(int how) = 0;

    virtual bool opened() const = 0;
    virtual bool closed() const = 0;

    virtual int getFileDescriptor() = 0;
};

} // namespace cld::transport

#endif
