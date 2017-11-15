#ifndef CLD_HTTP_RESPONSE_H_INCLUDED
#define CLD_HTTP_RESPONSE_H_INCLUDED

#include <string>
#include <map>
#include <memory>
#include <stdexcept>
#include "buffer.h"
#include "stream.h"

namespace cld::http {

class Response;

extern Response ReadToResponse(transport::Stream &stream);

class Response {
public:
    Response() {}
    Response(std::vector<std::byte> &response);

    int getStatus() const { return status; }
    const std::string &getHttpVersion() const { return http_version; }
    const std::string &getStatusText() const { return status_text; }
    const std::map<std::string, std::string> &getHeaders() const { return headers; }
    std::string operator[](const std::string &header) const {
        try {
            return headers.at(header);
        } catch (std::out_of_range &e) {
            return std::string();
        }
    }

    bool isOk() const { return status >= 200 && status <= 299; }
    bool isRedirected() const;
    
    std::ostream &debugInfo(std::ostream &os) const;

private:
    int status = 0;
    std::string http_version;
    std::string status_text;
    std::map<std::string, std::string> headers;
};

} // namespace cld::http

#endif
