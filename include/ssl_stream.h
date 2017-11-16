#ifndef CLD_SSL_STREAM_H_INCLUDED
#define CLD_SSL_STREAM_H_INCLUDED

#include "stream.h"
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

namespace cld::transport {

class SslStream : public Stream {
public:
    SslStream() : ctx(nullptr), ssl(nullptr), fd(-1) { }
    SslStream(const AddressInfo &address, bool blocking);
    ~SslStream() override;

    // ssl connect will block the thread
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
    static bool initialized;
    static void initialize();

    bool connected;

    SSL_CTX *ctx;
    SSL *ssl;
    int fd;
};

} // namespace cld::transport


#endif //CLD_SSL_STREAM_H_INCLUDED
