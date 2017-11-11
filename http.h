#ifndef HTTP_H_INCLUDED
#define HTTP_H_INCLUDED

#include "buffer.h"
#include "http_request.h"
#include "http_response.h"
#include "stream.h"
#include <map>
#include <string>
#include <numeric>

namespace cld::http {

extern std::pair<std::string, std::string> ParseHeaderString(const std::string & str);

}

#endif