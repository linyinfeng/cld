#include "http.h"

#include <string>
#include <map>
#include <cstdlib>

namespace cld {

namespace http {

Request::Request(
        const std::string &method,
        const std::string &resource,
        const std::map<std::string, std::string> headers,
        std::byte *data,
        std::size_t data_size) {
    method_ = method;
    resource_ = resource;
    headers_ = headers;
    data_ = new std::byte[data_size_];
    data_size_ = data_size;
    std::copy(data, data + data_size, data_);
}

Request::Request(const Request &other) {
    method_ = other.method_;
    resource_ = other.method_;
    headers_ = other.headers_;
    delete data_;
    data_ = new std::byte[other.data_size_];
    std::copy(other.data_, other.data_ + other.data_size_, data_);
    data_size_ = other.data_size_;
}

Request::Request(Request &&other) noexcept {
    swap(other);
}

Request &Request::operator= (Request other) {
    swap(other);
    return *this;
}

Request::~Request() noexcept {
    delete data_;
}

void Request::swap(Request &other) noexcept {
    using std::swap;
    swap(method_, other.method_);
    swap(resource_, other.resource_);
    swap(headers_, other.headers_);
    swap(other.data_, data_);
    swap(other.data_size_, data_size_);
}

} // namespace http

} // namespace cld
