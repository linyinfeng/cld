#ifndef CLD_HTTP_H_INCLUDED
#define CLD_HTTP_H_INCLUDED

#include <cstddef>
#include <map>
#include <algorithm>

namespace cld {

namespace http {

class Request {
public:
    Request(const std::string &method,
            const std::string &resource,
            const std::map<std::string, std::string> headers,
            std::byte *data,
            std::size_t data_size_);
    Request(const Request &other);
    Request(Request &&other) noexcept;
    Request &operator= (Request other);
    ~Request() noexcept;
    void swap(Request &other) noexcept;

    const std::string &getMethod() const { return method_; }
    const std::string &getResource() const { return resource_; }
    const std::map<std::string, std::string> &getHeaders() const { return headers_; }
    const std::byte *getData() const { return data_; }
    std::size_t getDataSize() const { return data_size_; }

    std::string &operator[] (const std::string &header) {
        return headers_[header];
    }

private:
    std::string method_;
    std::string resource_;
    std::map<std::string, std::string> headers_;
    std::byte *data_;
    std::size_t data_size_;
};

}

} // namespace cld

#endif
