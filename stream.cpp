#include "stream.h"

namespace cld {
namespace transport {

void Stream::write(const std::string &string) {
    write(reinterpret_cast<const std::byte *>(string.c_str()), string.length());
}

void Stream::write(const std::vector<std::byte> &buf) {
    write(buf.data(), buf.size());
}

std::size_t Stream::read(Buffer<std::byte> &buf) {
    return read(buf, buf.size());
}

std::size_t Stream::read(Buffer<std::byte> &buf, std::size_t limit) {
    if (limit > buf.size()) throw std::logic_error("read limit beyond buffer size");
    buf.setValid(0);
    std::size_t read_count = read(buf.data(), limit);
    buf.setValid(read_count);
    return read_count;
}

std::size_t Stream::read(char *buf, std::size_t count) {
    return read(reinterpret_cast<std::byte *>(buf), count);
}

std::string Stream::readLine() {
    std::string res;
    while (true) {
        char c;
        if (read(&c, 1) == 1 && c != '\n')
            res.push_back(c);
        else
            break;
    }
    return res;
}

} // namespace transport
} // namespace cld
