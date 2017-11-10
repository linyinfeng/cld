#include "request.h"

#include <string>
#include <map>
#include <cstdlib>

#include "url.h"
#include "options.h"

namespace cld {

namespace http {

const std::string kHttpVersion("1.1");

Request::Request(const std::string &method, const Url &url, const Options &options)
    : method_(method), resource_(url.resource()), data_(nullptr), data_size_(0) {
    headers_["Host"] = url.getHost();
    headers_["User-Agent"] = options.getUserAgent();
    for (const auto &extra_header : options.getExtraHeaders()) {
        headers_[extra_header.first] = extra_header.second;
    }
}

Request::Request(
        const std::string &method,
        const std::string &resource,
        const std::map<std::string, std::string> headers,
        std::byte *data = nullptr,
        std::size_t data_size = 0)
    : method_(method), resource_(resource), headers_(headers), data_size_(data_size) {
    if (data != nullptr) {
        data_ = new std::byte[data_size_];
        std::copy(data, data + data_size, data_);
    } else {
        data_ = nullptr;
    }
}

Request::Request(const Request &other) {
    method_ = other.method_;
    resource_ = other.method_;
    headers_ = other.headers_;
    if (data_ != nullptr)
        delete data_;
    if (other.data_ != nullptr) {
        data_ = new std::byte[other.data_size_];
        std::copy(other.data_, other.data_ + other.data_size_, data_);
        data_size_ = other.data_size_;
    }
}

Request::Request(Request &&other) noexcept {
    swap(other);
}

Request &Request::operator= (Request other) {
    swap(other);
    return *this;
}

Request::~Request() noexcept {
    if (data_ != nullptr)
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

void Request::setData(const std::byte *data, std::size_t size) {
    delete data_;
    data_ = new std::byte[size];
    std::copy(data, data + size, data_);
    data_size_ = size;
}

std::ostream &Request::debugInfo(std::ostream &os) const {
    os << "[Debug] Request:"
        << "\n\tMethod: " << method_
        << "\n\tResource: " << resource_
        << "\n\tHeader number: " << headers_.size();
    for (const auto &header : headers_) {
        os << "\n\tHeader: \"" << header.first << "\": \"" << header.second << "\"";
    }
    os << "\n\tData: " << data_
        << "\n\tData size: " << data_size_;
    return os << std::endl;
}

} // namespace http

} // namespace cld
