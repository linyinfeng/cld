#ifndef CLD_LENGTH_CONTROLLER_H_INCLUDED
#define CLD_LENGTH_CONTROLLER_H_INCLUDED

#include <fcntl.h>
#include <memory>
#include <list>
#include <map>
#include <tuple>
#include <iostream>
#include "worker.h"

namespace cld {

class LengthController {
public:
    LengthController(off_t size, off_t min);

    std::tuple<off_t, off_t> next();
    void add(const Worker *worker);

    // true indicates should be continue, false indicates should not continue
    bool afterRead(const Worker *worker);
    void workerStopped(const Worker *worker);
    off_t checkEnd(const Worker *worker);
    bool finished();

    std::size_t workerNumber();
    off_t remain();
    void debugInfo(std::ostream &os);

private:
    struct Fragment {
        Fragment(off_t b, off_t e, const Worker *w) : begin(b), end(e), worker(w) { }
        bool operator< (const Fragment &other);
        bool operator== (const Fragment &other);
        off_t splitLength() const;
        off_t begin;
        off_t end;
        const Worker *worker;
    };

private:
    std::size_t min;

    std::list<Fragment> length_monitor;
    std::map<const Worker *, Fragment *> node_finder;
};

} // namespace cld

#endif
