#include "stream.h"

namespace cld::transport {

void Stream::write(const std::byte *buf, std::size_t size) {
    _write(buf, size);
}

void Stream::write(const std::string &string) {
    _write(reinterpret_cast<const std::byte *>(string.c_str()), string.length());
}

void Stream::write(const std::vector<std::byte> &buf) {
    _write(buf.data(), buf.size());
}

std::size_t Stream::read(std::byte *buf, std::size_t size) {
    return _read(buf, size);
}

std::size_t Stream::read(Buffer<std::byte> &buf) {
    buf.clear();
    auto count = _read(buf.data(), buf.maxRead());
    buf.makeValid(count);
    return count;
}

std::size_t Stream::read(char *buf, std::size_t count) {
    return _read(reinterpret_cast<std::byte *>(buf), count);
}

std::string Stream::readLine() {
    std::string res;
    while (true) {
        char c;
        if (_read(reinterpret_cast<std::byte *>(&c), 1) == 1) {
            res.push_back(c);
            if (c == '\n')
                break;
        } else {
            break;
        }
    }
    return res;
}

} // namespace cld::transport
