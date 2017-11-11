#ifndef CLD_STREAM_H_INCLUDED
#define CLD_STREAM_H_INCLUDED

#include <cstddef>
#include <string>
#include <vector>
#include "address_info.h"
#include "buffer.h"

namespace cld {
namespace transport {

// A network TcpTcpStream with a seperated buffer
class Stream {
public:
    Stream() { }
    virtual ~Stream() { };

    // all read method read system buffer to empty
    virtual std::size_t read(std::byte *buf, std::size_t count) = 0;
    virtual std::size_t read(Buffer<std::byte> &buf);
    virtual std::size_t read(Buffer<std::byte> &buf, std::size_t limit);
    virtual std::size_t read(char *buf, std::size_t count);
    virtual std::string readLine();
    // all write method must wirte all data
    virtual void write(const std::byte *buf, std::size_t size) = 0;
    virtual void write(const std::string &string);
    virtual void write(const std::vector<std::byte> &buf);

    virtual void connect(const AddressInfo &address) = 0;
    virtual void close() = 0;

    virtual bool opened() const = 0;
    virtual bool closed() const = 0;
};

} // namespace transport
} // namespace cld

#endif
