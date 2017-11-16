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
    connected = false;
    ssl = nullptr;
    connect(address, blocking);
}

void SslStream::connect(const AddressInfo &address, bool blocking) {
    if (!initialized)
        initialize();
    for (const struct addrinfo &ai : address) {
        try {
            fd = wrapper::Socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);

            if (!blocking) {
            // set non-blocking
                int flags = fcntl(fd, F_GETFL);
                if (flags == -1)
                    throw std::system_error(errno, std::system_category());
                if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
                    throw std::system_error(errno, std::system_category());
            }
            if (::connect(fd, ai.ai_addr, ai.ai_addrlen) != 0 && errno != EINPROGRESS) {
                throw std::system_error(errno, std::system_category());
            }

            if (blocking) {
                continueConnect();
            }
            break;
        }
        catch (std::exception &e) {
            close();
            continue;
        }
    }
}


bool SslStream::continueConnect() {
    if (connected) return false;

    if (ssl == nullptr) {
        const SSL_METHOD *method = SSLv23_method();
        if (method == nullptr) throw std::runtime_error("Failed to create SSL method");
        ctx = SSL_CTX_new(method);
        if (ctx == nullptr) throw std::runtime_error("Failed to create SSL context");
        ssl = SSL_new(ctx);
        if (ssl == nullptr) throw std::runtime_error("Failed to create SSL socket");
        SSL_set_fd(ssl, fd);
    }

    int res = 0;
    if ((res = SSL_connect(ssl)) != 1) {
        auto err = SSL_get_error(ssl, res);
        switch (err) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
            case SSL_ERROR_WANT_CONNECT:
                return true;
            default:
                throw std::runtime_error(std::string("Failed to connect with SSL: ") + ERR_error_string(err, nullptr));
        }
    } else {
        connected = true;
        return false;
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
    (void)how;
}

bool SslStream::opened() const {
    if (fd == -1 || !connected) return false;
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
    while (true) {
        int read_count;
        if ((read_count = SSL_read(ssl, buf, static_cast<int>(size))) < 0) {
            switch (SSL_get_error(ssl, read_count)) {
                case SSL_ERROR_NONE:
                case SSL_ERROR_ZERO_RETURN:
                    return 0;
                case SSL_ERROR_WANT_READ:
                case SSL_ERROR_WANT_WRITE:
                    throw StreamException(StreamException::Exception::kAgain);
                default:
                    throw std::runtime_error("SSL error");
            }
        }
        return static_cast<std::size_t>(read_count);
    }
}

std::size_t SslStream::write(const std::byte *buf, std::size_t size) {
    while (true) {
        int write_count;
        if ((write_count = SSL_write(ssl, buf, static_cast<int>(size))) < 0) {
            switch (SSL_get_error(ssl, write_count)) {
                case SSL_ERROR_NONE:
                case SSL_ERROR_ZERO_RETURN:
                    return 0;
                case SSL_ERROR_WANT_WRITE:
                case SSL_ERROR_WANT_READ:
                    throw StreamException(StreamException::Exception::kAgain);
                default:
                    throw std::runtime_error("SSL error");
            }
        }
        return static_cast<std::size_t>(write_count);
    }
}

} // namespace cld::transport