#include "stream.h"
#include "tcp_stream.h"
#include "ssl_stream.h"
#include <memory>
#include <stdexcept>

namespace cld::transport {

std::shared_ptr<Stream> CreateStream(const std::string &scheme, const AddressInfo &addr, bool blocking) {
    if (scheme == "http") {
        return std::make_shared<TcpStream>(addr, blocking);
    } else if (scheme == "https") {
        return std::make_shared<SslStream>(addr,blocking);
    } else {
        throw std::logic_error("Unsupported scheme");
    }
}

}