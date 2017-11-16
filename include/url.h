#ifndef CLD_URL_H_INCLUDED
#define CLD_URL_H_INCLUDED

#include <string>
#include <iostream>

namespace cld {

class Url
{
    friend std::istream &operator>> (std::istream &is, Url &url);

public:
    Url() {}
    explicit Url(const std::string &url_string); // Parse url

    std::string string() const;

    // getaddrinfo parameters
    const std::string &service() const; // port or scheme
    const std::string &node() const; // host
    std::string resource() const; // path, query and fragment

    const std::string &getScheme() const { return scheme; }
    const std::string &getHost() const { return host; }
    const std::string &getPort() const { return port; }
    const std::string &getPath() const { return path; }
    const std::string &getQuery() const { return query; }
    const std::string &getFragment() const { return fragment; }

    void setScheme(const std::string &scheme) { this->scheme = scheme; }
    void setHost(const std::string &host) { this->host = host; }
    void setPort(const std::string &port) { this->port = port; }
    void setPath(const std::string &path) { this->path = path; }
    void setQuery(const std::string &query) { this->query = query; }
    void setFragment(const std::string &fragment) { this->fragment = fragment; }

    std::ostream & debugInfo(std::ostream &os);

private:
    std::string scheme;
    /* Authority */
    std::string host;
    std::string port;
    /* Resouces */
    std::string path;
    std::string query;
    std::string fragment;
};

extern std::istream &operator>> (std::istream &is, Url &url);
extern std::ostream &operator<< (std::ostream &os, Url &url);

} // namespace cld

#endif
