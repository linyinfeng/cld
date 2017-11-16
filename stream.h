#ifndef CLD_STREAM_H_INCLUDED
#define CLD_STREAM_H_INCLUDED

#include <cstddef>
#include <string>
#include <vector>
#include "address_info.h"
#include "buffer.h"

namespace cld::transport {

class Stream;

extern std::shared_ptr<Stream> CreateStream(const std::string &scheme, const AddressInfo &addr, bool blocking);

// A network TcpTcpStream with a seperated buffer
class Stream {
public:
    Stream() = default;
    virtual ~Stream() = default;

    virtual std::size_t write(const std::byte *buf, std::size_t size) = 0;
    virtual std::size_t read(std::byte *buf, std::size_t count) = 0;

    virtual void connect(const AddressInfo &address, bool blocking) = 0;
    virtual void close() = 0;
    virtual void shutdown(int how) = 0;

    virtual bool opened() const = 0;
    virtual bool closed() const = 0;

    virtual int getFileDescriptor() = 0;
};

class StreamException {
public:
    enum class Exception {
        kAgain
    } exception;
    explicit StreamException(Exception e) : exception(e) { }
};

} // namespace cld::transport

#endif
