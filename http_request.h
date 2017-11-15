#ifndef CLD_HTTP_REQUEST_H_INCLUDED
#define CLD_HTTP_REQUEST_H_INCLUDED

#include "url.h"
#include "options.h"
#include "stream.h"

#include <vector>
#include <iostream>
#include <cstddef>
#include <map>
#include <algorithm>

namespace cld::http {

class Request;

extern void WriteRequest(const Request &request, transport::Stream &stream);

class Request {
public:
    Request() { }
    Request(const std::string &method, const Url &url, const Options &options);
    Request(const std::string &method, const std::string &resource, const std::map<std::string, std::string> headers,
            const std::vector<std::byte> &body = std::vector<std::byte>())
        : method(method), resource(resource), headers(headers), body(body) { }

    const std::string &getMethod() const { return method; }
    const std::string &getResource() const { return resource; }
    const std::map<std::string, std::string> &getHeaders() const { return headers; }
    std::string getNonBody() const;
    const std::vector<std::byte> &getBody() const { return body; }
    void setMethod(const std::string &method) { this->method = method; }
    void setResource(const std::string &resource) { this->resource = resource; }
    void setbody(const std::vector<std::byte> &body) { this->body = body; }

    std::string &operator[](const std::string &header) { return headers[header]; }
    const std::string &at(const std::string &header) const { return headers.at(header); }
    const std::string &operator[](const std::string &header) const { return headers.at(header); }
    std::byte &operator[](std::size_t pos) { return body[pos]; }
    const std::byte &operator[](std::size_t pos) const { return body[pos]; }

    std::vector<std::byte> request() const;

    std::ostream &debugInfo(std::ostream &os) const;

private:
    std::string method;
    std::string resource;
    std::map<std::string, std::string> headers;
    std::vector<std::byte> body;

    static const std::string kHttpVersion;
};

} // namespace cld::http

#endif
