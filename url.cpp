#include "url.h"
#include <regex>
#include <stdexcept>

namespace cld {

Url::Url(const std::string & url_string)
{
    std::smatch matches;
    static std::regex normal_url_regex(
        //      Scheme:         / / host            :port      /path                                  ?query                                    #fragment
        R"regex((?:([\w+.-]*):)?\/\/([A-Za-z0-9.-]*)(?::(\d+))?(?:\/([a-zA-Z0-9._~!$&'()*+,;=:@%-]*))?(?:\?([a-zA-Z0-9._~!$&'()*+,;=:@\/?%-]*))?(?:#([a-zA-Z0-9._~!$&'()*+,;=:@\/?%-]*))?)regex"
    );
    static std::regex ipv6_url_regex(
        //      Scheme:         / /   ip                :port      /path                                  ?query                                    #fragment
        R"regex((?:([\w+.-]*):)?\/\/\[([A-Za-z0-9:%]*)\](?::(\d+))?(?:\/([a-zA-Z0-9._~!$&'()*+,;=:@%-]*))?(?:\?([a-zA-Z0-9._~!$&'()*+,;=:@\/?%-]*))?(?:#([a-zA-Z0-9._~!$&'()*+,;=:@\/?%-]*))?)regex"
    );

    if (std::regex_match(url_string, matches, normal_url_regex) ||
        std::regex_match(url_string, matches, ipv6_url_regex)) {
        scheme = matches[1];
        host = matches[2];
        port = matches[3];
        path = matches[4];
        query = matches[5];
        fragment = matches[6];
    }
    else {
        throw std::runtime_error("Invalid URL \"" + url_string + "\"");
    }
}

std::string Url::string() const
{
    return (!scheme.empty() ? scheme + ":" : "") + "//" + host +
        (!port.empty() ? ":" + port : "") + resource();
}

const std::string & Url::service() const
{
    if (port.empty())
        return scheme;
    else
        return port;
}

const std::string & Url::node() const
{
    return host;
}

std::string Url::resource() const
{
    return "/" + path + (!query.empty() ? "?" + query : "") + (!fragment.empty() ? "#" + fragment : "");
}

std::ostream & Url::debugInfo(std::ostream & os)
{
    return os
        << "[Debug] " << "URL:"
        << "\n\tString: " << string()
        << "\n\tService: " << service()
        << "\n\tNode: " << node()
        << "\n\tResouce: " << resource()
        << "\n\tScheme: " << scheme
        << "\n\tHost: " << host
        << "\n\tPort:" << port
        << "\n\tPath: " << path
        << "\n\tQuery: " << query
        << "\n\tFragment: " << fragment
        << std::endl;;
}

std::istream & operator>>(std::istream & is, Url & url)
{
    std::string url_string;
    is >> url_string;
    url = Url(url_string);
    return is;
}

std::ostream & operator<<(std::ostream & os, Url & url)
{
    return os << url.string();
}

} // namespace cld
