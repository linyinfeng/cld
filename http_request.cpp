#include "http_request.h"

#include <string>
#include <map>
#include <cstdlib>
#include <sstream>

#include "url.h"
#include "options.h"
#include "stream.h"

namespace cld {
namespace http {

const std::string Request::kHttpVersion("1.1");

void Write(const Request &request, transport::Stream &s) {
    s.write(request.getNonBody() + "\r\n");
    s.write(request.getBody());
}

Request::Request(const std::string &method, const Url &url, const Options &options)
    : method(method), resource(url.resource()), body() {
    headers["Host"] = url.getHost();
    headers["User-Agent"] = options.getUserAgent();
    for (const auto &extra_header : options.getExtraHeaders()) {
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

} // namespace http
} // namespace cld
