#ifndef CLD_TcpStream_H_INCLUDED
#define CLD_TcpStream_H_INCLUDED

#include <cstddef>
#include "address_info.h"
#include "buffer.h"

namespace cld {

namespace transport {

// A network TcpTcpStream with a seperated buffer
class TcpStream {
public:
    TcpStream() : fd(-1) { }
    TcpStream(const AddressInfo &address);
    ~TcpStream();

    int fileDescriptor();

    void read(Buffer<std::byte> &buf);
    void readFill(Buffer<std::byte> &buf);
    std::string readLine();
    void write(Buffer<std::byte> &buf);
    void write(std::byte *buf, std::size_t size);

    void connect(const AddressInfo &address);
    void close();

    bool opened();
    bool closed();

private:
    int fd;
};

} // namespace transport

} // namespace cld

#endif
