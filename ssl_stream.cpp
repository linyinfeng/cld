#include "ssl_stream.h"
#include "wrapper.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace cld::transport {

bool SslStream::initialized(false);

void SslStream::initialize() {
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_all_algorithms();
    initialized = true;
}

SslStream::SslStream(const AddressInfo &address, bool blocking) {
    connect(address, blocking);
}

void SslStream::connect(const AddressInfo &address, bool blocking) {
    if (!initialized)
        initialize();
    for (const struct addrinfo &ai : address) {
        try {
            fd = wrapper::Socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
            wrapper::Connect(fd, ai.ai_addr, ai.ai_addrlen);
            const SSL_METHOD* method = SSLv23_method();
            if(method == nullptr) throw std::exception();
            ctx = SSL_CTX_new(method);
            if (ctx == nullptr) throw std::exception();
//            SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
            ssl = SSL_new(ctx);
            if (ssl == nullptr) throw std::exception();
            SSL_set_fd(ssl, fd);
            if (SSL_connect(ssl) <= 0) throw std::exception();

            if (!blocking) {
                int flags = fcntl(fd, F_GETFL);
                if (flags == -1)
                    throw std::system_error(errno, std::system_category());
                if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
                    throw std::system_error(errno, std::system_category());
            }
            break;
        }
        catch (std::exception &e) {
            close();
            continue;
        }
    }
}

SslStream::~SslStream() {
    close();
}

void SslStream::close() {
    if (ssl != nullptr) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        ssl = nullptr;
    }
    if (ctx != nullptr) {
        SSL_CTX_free(ctx);
        ctx = nullptr;
    }
    if (fd != -1) {
        wrapper::Close(fd);
        fd = -1;
    }
}

void SslStream::shutdown(int how) {
    // do nothing
}

bool SslStream::opened() const {
    if (fd == -1) return false;
    int result;
    socklen_t result_len = sizeof(result);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) {
        return false;
    }
    if (result != 0) {
        return false;
    }
    return ssl != nullptr;
}

bool SslStream::closed() const {
    return !opened();
}

std::size_t SslStream::read(std::byte *buf, std::size_t size) {
    do {
        int res = SSL_read(ssl, buf, static_cast<int>(size));
        if (res <= 0) {
            if (SSL_get_error(ssl, res) == SSL_ERROR_WANT_READ) {
                continue;
            } else {
                throw std::runtime_error("SSL error");
            }
        }
        return static_cast<std::size_t>(res);
    } while (true);
}

std::size_t SslStream::write(const std::byte *buf, std::size_t size) {
    do {
        int res = SSL_write(ssl, buf, static_cast<int>(size));
        if (res <= 0) {
            if (SSL_get_error(ssl, res) == SSL_ERROR_WANT_WRITE) {
                continue;
            } else {
                throw std::runtime_error("SSL error");
            }
        }
        return static_cast<std::size_t>(res);
    } while (true);
}

} // namespace cld::transport