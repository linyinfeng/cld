#ifndef CLD_CLD_H_INCLUDED
#define CLD_CLD_H_INCLUDED

#include "options.h"
#include "url.h"
#include "address_info.h"
#include "http.h"
#include <string>

namespace cld {

void PrintHelp();

void Cld(Options &options, Url &url);



} // namespace cld

#endif // CLD_CLD_H_INCLUDED
