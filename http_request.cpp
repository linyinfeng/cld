#include <string>
#include <map>
#include <cstdlib>
#include <sstream>

#include "http_request.h"

#include "url.h"
#include "options.h"
#include "stream.h"

namespace cld::http {

void WriteRequest(const Request &request, transport::Stream &stream) {
    auto data = request.request();
    stream.write(data.data(), data.size());
}

const std::string Request::kHttpVersion("1.1");

Request::Request(const std::string &method, const Url &url, const Options &options)
    : method(method), resource(url.resource()), body() {
    headers["Host"] = url.getHost();
    headers["User-Agent"] = options.getUserAgent();
    for (const auto &extra_header : headers) {
        headers[extra_header.first] = extra_header.second;
    }
}

std::string Request::getNonBody() const {
    std::ostringstream ss;
    ss << method << " " << getResource() << " HTTP/" << kHttpVersion << "\r\n";
    for (const auto &header : headers)
        ss << header.first << ": " << header.second << "\r\n";
    ss << "\r\n";
    return ss.str();
}

std::ostream &Request::debugInfo(std::ostream &os) const {
    os << "[Debug] Request:"
        << "\n\tMethod: " << method
        << "\n\tResource: " << resource
        << "\n\tHeader number: " << headers.size();
    for (const auto &header : headers) {
        os << "\n\tHeader: \"" << header.first << "\": \"" << header.second << "\"";
    }
    os << "\n\tBody size: " << body.size();
    return os << std::endl;
}

std::vector<std::byte> Request::request() const {
    std::vector<std::byte> res;
    for (char c : getNonBody()) {
        res.push_back(static_cast<std::byte>(c));
    }
    res.push_back(static_cast<std::byte>('\r'));
    res.push_back(static_cast<std::byte>('\n'));
    for (std::byte b : body) {
        res.push_back(b);
    }
    return res;
}

} // namespace cld::http
