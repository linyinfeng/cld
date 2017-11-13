#ifndef CLD_CLD_H_INCLUDED
#define CLD_CLD_H_INCLUDED

#include "options.h"
#include "url.h"
#include "address_info.h"
#include "http.h"

namespace cld {

void PrintHelp();

void Cld(const Options &options, const Url &initial_url);

void FollowRedirects(); // TODO Follow redirect
void SingleConnectionDownload(const AddressInfo &address, const std::string &scheme,
                             const http::Request &request, int file);
void MultiConnectionDownload(const AddressInfo &address, const std::string &scheme,
                             const http::Request &request, int file, std::size_t size, int connection_number);
//void Download(AddressInfo address_info, http::Request request);        // TODO Download

} // namespace cld

#endif // CLD_CLD_H_INCLUDED
