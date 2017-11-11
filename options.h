#ifndef CLD_OPTIONS_H_INCLUDED
#define CLD_OPTIONS_H_INCLUDED

#include "version.h"

#include <string>
#include <iostream>
#include <map>

namespace cld {

class Options {
private:
    int connections_number;         // default connections number is 8
    std::string user_agent;         // default user agent is "Cld"
    std::string output_path;        // default output_path is base on url_string
    std::map<std::string, std::string> extra_headers;

    static std::pair<std::string, std::string> parseHeaderString(const std::string &str);

public:
    Options() {
        setConnectionsNumber(8);
        setUserAgent("Cld/" + kCldVersion);
    }

    int getConnectionsNumber() const { return connections_number; }
    const std::string &getUserAgent() const { return user_agent; }
    const std::string &getOutputPath() const { return output_path; }
    const std::map<std::string, std::string> &getExtraHeaders() const { return extra_headers; }

    Options &setConnectionsNumber(int n);
    Options &setUserAgent(const std::string &str);
    Options &addExtraHeader(const std::string &str);
    Options &addExtraHeader(const std::string &name, const std::string &value);
    Options &setOutputPath(const std::string &str);

    std::ostream &debugInfo(std::ostream &os);
};

} // namespace cld

#endif
