#include "length_controller.h"
#include <algorithm>

namespace cld {

LengthController::LengthController(off_t size, off_t min)
        : min(min), length_monitor{ {0, size - 1, nullptr} } { }

std::tuple<off_t, off_t> LengthController::next() {
    length_monitor.sort();
    Fragment f = length_monitor.back();
    if (f.worker != nullptr) {
        if ((f.end - f.begin) / 2 > static_cast<off_t>(min))
            return std::make_tuple(f.begin + (f.end - f.begin) / 2, f.end);
        else
            return std::make_tuple(0, 0);
    } else {
        return std::make_tuple(f.begin, f.end);
    }
}

void LengthController::add(const Worker *worker) {
    length_monitor.sort();
    Fragment &f = length_monitor.back();
    if (f.worker != nullptr) {
        if ((f.end - f.begin) / 2 > static_cast<off_t>(min)) {
            auto mid = f.begin + (f.end - f.begin) / 2, end = f.end;
            f.end = mid;
            length_monitor.emplace_back(mid, end, worker);
            node_finder[worker] = &length_monitor.back();
        } else {
            throw std::runtime_error("Add work unexpectedly");
        }
    } else {
        f.worker = worker;
        node_finder[worker] = &length_monitor.back();

    }
}

bool LengthController::afterRead(const Worker *worker) {
    Fragment *f = node_finder.at(worker);
    f->begin = worker->getOffset();
    if (f->end <= f->begin) {
        std::cout << "[Debug] Controller report worker " << worker << " finished" << std::endl;
        length_monitor.remove(*f); // finish worker's fragment
        node_finder.erase(worker);
        return false; // require stop read
    } else {
        return true;
    }
}

void LengthController::workerStopped(const Worker *worker) {
    try {
        Fragment *f = node_finder.at(worker);
        f->begin = worker->getOffset();
        if (f->end <= f->begin) {
            length_monitor.remove(*f); // finish worker's fragment
        } else {
            f->worker = nullptr;
        }
        node_finder.erase(worker);
    } catch (std::out_of_range &e) {
        std::cout << "[Debug] Stopping of worker " << worker << " confirmed" << std::endl;
    }
}

off_t LengthController::checkEnd(const Worker *worker) {
    return node_finder.at(worker)->end;
};

bool LengthController::finished() {
    if (length_monitor.empty()) {
        if (node_finder.empty()) {
            return true;
        }
        throw std::runtime_error("Bad bound in length controller");
    }
    return false;
}

void LengthController::debugInfo(std::ostream &os) {
    os << "[Debug] Length controller:";
    os << "\n\tList size: " << length_monitor.size();
    for (const auto &f : length_monitor) {
        os << "\n\t" << &f << std::setw(20) << f.begin << " " <<
            std::setw(20) << f.end << " " << f.worker;
    }
    os << "\n\tMap size: " << node_finder.size();
    for (const auto &t : node_finder) {
        os << "\n\t" << t.first << " - " << t.second << " "
            << std::setw(20) << t.second->begin << " "
            << std::setw(20) << t.second->end << " "
            << t.second->worker;
    }
    os << std::endl;
}

std::size_t LengthController::workerNumber() {
    return node_finder.size();
}

off_t LengthController::remain() {
    off_t res = 0;
    for (const auto &f : length_monitor) {
        res += f.end - f.begin;
    }
    return res;
}

bool LengthController::Fragment::operator<(const LengthController::Fragment &other) {
    return splitLength() < other.splitLength();
}

off_t LengthController::Fragment::splitLength() const {
    if (worker == nullptr) {
        return end - begin;
    } else {
        return (end - begin) / 2;
    }
}

bool LengthController::Fragment::operator==(const LengthController::Fragment &other) {
    return begin == other.begin && end == other.end && worker == other.worker;
}

} // namespace cld
