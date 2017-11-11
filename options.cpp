#include "options.h"
#include <string>
#include <stdexcept>
#include <map>
#include <regex>

namespace cld {

std::pair<std::string, std::string> Options::parseHeaderString(const std::string & str)
{
    std::regex regex(R"regex(([a-zA-Z0-9!#$%&'*+.^_`|~-]+)\s*:\s*([^\r\n]*))regex");
    std::smatch matches;
    if (std::regex_match(str, matches, regex)) {
        return std::pair<std::string, std::string>(matches[1], matches[2]);
    }
    else {
        throw std::runtime_error("Invalid header \"" + str + "\"");
    }
}

Options &Options::setConnectionsNumber(int n)
{
    if (n > 0)
        connections_number = n;
    else
        throw std::invalid_argument("Invalid connections Number option");
    return *this;
}

Options & Options::setUserAgent(const std::string & str)
{
    if (str.empty()) throw std::runtime_error("Empty user agent");
    user_agent = str;
    return *this;
}

Options & Options::addExtraHeader(const std::string & str)
{
    if (!str.empty()) {
        extra_headers.insert(parseHeaderString(str));
    }
    else {
        throw std::invalid_argument("Empty header");
    }
    return *this;
}

Options & Options::addExtraHeader(const std::string &name, const std::string &value) {
    if (name.empty() || value.empty()) {
        throw std::invalid_argument("Empty header name or value");
    } else {
        extra_headers.insert(std::make_pair(name, value));
    }
    return *this;
}

Options & Options::setOutputPath(const std::string & str)
{
    if (!str.empty())
        output_path = str;
    else
        throw std::invalid_argument("Empty output path");
    return *this;
}

std::ostream &Options::debugInfo(std::ostream &os)
{
    os << "[Debug] Options:"
        << "\n\tConnections number: " << connections_number
        << "\n\tUser agent: " << user_agent
        << "\n\tOutput path: " << output_path;
    for (const auto &p : extra_headers) {
        os << "\n\tExtra header: \"" << p.first << "\": \"" << p.second << "\"";
    }
    return os << std::endl;
}

} // namespace cld
