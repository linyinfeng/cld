#include "http_response.h"
#include "http.h"
#include "buffer.h"
#include <regex>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <numeric>

namespace cld::http {

void Read(Response &response, transport::Stream &s) {
    std::string line = s.readLine();
    static std::regex start_line_regex(R"regex(HTTP\/([\d.]+)\s+([\d]{3})\s+([^\r\n]+)\r\n)regex");
    std::smatch matches;
    if (std::regex_match(line, matches, start_line_regex)) {
        response.http_version = matches[1];
        std::istringstream(matches[2]) >> response.status;
        response.status_text = matches[3];
    } else {
        throw std::runtime_error("Invalid response start line");
    }
    while ( (line = s.readLine()) != "\r\n") {
        auto header = ParseHeaderString(line);
        response.headers[header.first] = header.second;
    }
}

bool Response::isRedirected() const {
    // TODO
    return false;
}

std::ostream &Response::debugInfo(std::ostream &os) const {
    os << "[Debug] Response:"
        << "\n\tHTTP version: " << http_version
        << "\n\tStatus: " << status
        << "\n\tStatus text: " <<status_text;
    for (const auto &header : headers) {
        os << "\n\tHeader: \"" << header.first << "\": \"" << header.second << "\"";
    }
    return os << std::endl;
}

} // namespace cld::http