#include "stream.h"

namespace cld {

namespace transport {

std::string Stream::readLine() {
    std::string res;
    Buffer<std::byte> buf(1);
    while (true) {
        readFill(buf);
        if (buf.valid() == 1) {
            char c = *reinterpret_cast<char *>(buf.data());
            res.push_back(c);
            if (c == '\n')
                break;
        }
    }
    return res;
}

}

}
