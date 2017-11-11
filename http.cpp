#include "http.h"
#include <string>
#include <map>
#include <regex>

namespace cld::http {

std::pair<std::string, std::string> ParseHeaderString(const std::string & str)
{
    std::regex regex(R"regex(([a-zA-Z0-9!#$%&'*+.^_`|~-]+)\s*:\s*([^\r\n]*)(:?\r\n)?)regex");
    std::smatch matches;
    if (std::regex_match(str, matches, regex)) {
        return std::pair<std::string, std::string>(matches[1], matches[2]);
    }
    else {
        throw std::runtime_error("Invalid header \"" + str + "\"");
    }
}

}
