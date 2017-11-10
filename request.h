#ifndef CLD_REQUEST_H_INCLUDED
#define CLD_REQUEST_H_INCLUDED

#include "url.h"
#include "options.h"

#include <iostream>
#include <cstddef>
#include <map>
#include <algorithm>

namespace cld {

namespace http {

class Request {
public:
    Request(const std::string &method, const Url &url, const Options &options);
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
    void setMethod(const std::string &method) { method_ = method; }
    const std::string &getResource() const { return resource_; }
    void setResource(const std::string &resource) { resource_ = resource; }
    const std::map<std::string, std::string> &getHeaders() const { return headers_; }
    const std::byte *getData() const { return data_; }
    std::size_t getDataSize() const { return data_size_; }
    void setData(const std::byte *data, std::size_t size);

    std::string &operator[] (const std::string &header) {
        return headers_[header];
    }

    std::ostream &debugInfo(std::ostream &os) const;

private:
    std::string method_;
    std::string resource_;
    std::map<std::string, std::string> headers_;
    std::byte *data_;
    std::size_t data_size_;

    static const std::string kHttpVersion;
};

}

} // namespace cld

#endif
