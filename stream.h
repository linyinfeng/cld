#ifndef CLD_STREAM_H_INCLUDED
#define CLD_STREAM_H_INCLUDED

#include "address_info.h"
#include "buffer.h"

namespace cld {

// A network stream with a seperated buffer
class Stream {
public:
    Stream() : fd(-1) { }
    Stream(const AddressInfo &address);
    ~Stream();

    int fileDescriptor();

    void read(Buffer<std::byte> &buf);
    void readFill(Buffer<std::byte> &buf);
    void write(Buffer<std::byte> &buf);
    void write(char *buf);

    void connect(const AddressInfo &address);
    void close();

    bool opened();
    bool closed();

private:
    int fd;
};

}

#endif
