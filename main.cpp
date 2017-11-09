// Standard headers
#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>

// Linux headers
#include <getopt.h>

// Cld headers
#include "cld.h"
#include "url.h"
#include "options.h"

int main(int argc, char **argv) {
    cld::Options options;

    while (true) {
        static struct option long_options[] = {
            { "num-connections", required_argument, nullptr, 'n' },
            { "output",          required_argument, nullptr, 'o' },
            { "user-agent",      required_argument, nullptr, 'U' },
            { "header",          required_argument, nullptr, 'H' },
            { "help",            no_argument,       nullptr, 'h' },
            { nullptr,           0,                 nullptr, 0   }
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "n:o:U:H:h", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
        case 0: break; // never occurs
        case 'n':
        {
            int tmp;
            std::istringstream ss(optarg);
            if (ss >> tmp)
                options.setConnectionsNumber(tmp);
            else
                throw std::invalid_argument("Invalid connections number");
            break;
        }
        case 'o': options.setOutputPath(optarg); break;
        case 'U': options.setUserAgent(optarg); break;
        case 'H': options.addExtraHeader(optarg); break;
        case 'h': cld::PrintHelp(); std::exit(EXIT_SUCCESS);
        case '?': cld::PrintHelp(); std::exit(EXIT_SUCCESS);
        default: throw std::invalid_argument("Unknow option");
        }
    }

    if (optind != argc - 1) {
        // optind doesn't point to the last parameter
        cld::PrintHelp();
        std::exit(EXIT_SUCCESS);
    }

    options.debugInfo(std::cout);
    cld::Url url(argv[optind]);
    url.debugInfo(std::cout);

    // start cld
    cld::Cld(options, url);

    exit(0);
}
