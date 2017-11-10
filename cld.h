#ifndef CLD_CLD_H_INCLUDED
#define CLD_CLD_H_INCLUDED

#include "options.h"
#include "url.h"

namespace cld {

void PrintHelp();

void Cld(const Options &options, const Url &initial_url);

void FollowRedirects(); // TODO Follow redirects
void Download();        // TODO Download

}

#endif // CLD_CLD_H_INCLUDED
