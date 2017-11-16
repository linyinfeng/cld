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

Response ReadToResponse(transport::Stream &stream) {
    std::vector<std::byte> response_data;
    char c, last_char = '\0'; auto p = reinterpret_cast<std::byte *>(&c);
    while (stream.read(p, 1) == 1) {
        response_data.push_back(*p);
        if (last_char == '\n' && c == '\r') {
            if (stream.read(p, 1) == 1 && c == '\n') {
                // read \n
                response_data.push_back(*p);
                http::Response response(response_data);
                return response;
                break;
            } else {
                throw std::runtime_error("No \\n follows \\r in stream");
            }
        }
        last_char = c;
    }
    throw std::runtime_error("Failed to read to response");
}

Response::Response(std::vector<std::byte> &response) {
    response.push_back(static_cast<std::byte>('\0'));
    std::istringstream ss(reinterpret_cast<const char *>(response.data()));

    std::string line;
    std::getline(ss, line);

    static std::regex start_line_regex(R"regex(HTTP\/([\d.]+)\s+([\d]{3})\s+([^\r\n]+)\r)regex");
    std::smatch matches;
    if (std::regex_match(line, matches, start_line_regex)) {
        http_version = matches[1];
        std::istringstream(matches[2]) >> status;
        status_text = matches[3];
    } else {
        throw std::runtime_error("Invalid response start line");
    }
    while (true) {
        std::getline(ss, line);
        if (line[0] == '\r')
            break;
        auto header = ParseHeaderString(line);

        if (header.first == "Set-Cookie") {
            std::string old = headers[header.first];
            static std::regex regex(R"regex(.*;\s*$)regex");
            if (!old.empty() && !std::regex_match(old, regex))
                headers[header.first] += "; ";
            headers[header.first] += header.second;
        } else {
            headers[header.first] = header.second;
        }
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
