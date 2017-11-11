#ifndef CLD_ADDRESS_INFO_H_INCLUDED
#define CLD_ADDRESS_INFO_H_INCLUDED

#include <string>
#include <stdexcept>
#include <iostream>

#include <netdb.h>
#include "url.h"

namespace cld {

class AddressInfo;

class AddressInfoIterator {
public:
    AddressInfoIterator() : ai(nullptr) { }
    AddressInfoIterator(struct addrinfo *p) : ai(p) { }
    AddressInfoIterator(const AddressInfoIterator &other) : ai(other.ai) { }
    AddressInfoIterator &operator= (const AddressInfoIterator &other) {
        ai = other.ai;
        return *this;
    }

    struct addrinfo &operator* () { return *ai; }
    struct addrinfo *operator-> () { return ai; }
    bool operator== (const AddressInfoIterator &other) { return ai == other.ai; }
    bool operator!= (const AddressInfoIterator &other) { return ai != other.ai; }

    AddressInfoIterator &operator++();
    AddressInfoIterator operator++(int);

private:
    struct addrinfo *ai;
};

class AddressInfoConstIterator {
public:
    AddressInfoConstIterator() : ai(nullptr) { }
    AddressInfoConstIterator(const struct addrinfo *p) : ai(p) { }
    AddressInfoConstIterator(const AddressInfoConstIterator &other) : ai(other.ai) { }
    AddressInfoConstIterator &operator= (const AddressInfoConstIterator &other) {
        ai = other.ai;
        return *this;
    }

    const struct addrinfo &operator* () { return *ai; }
    const struct addrinfo *operator-> () { return ai; }
    bool operator== (const AddressInfoConstIterator &other) { return ai == other.ai; }
    bool operator!= (const AddressInfoConstIterator &other) { return ai != other.ai; }

    AddressInfoConstIterator &operator++();
    AddressInfoConstIterator operator++(int);

private:
    const struct addrinfo *ai;
};

class AddressInfo {
public:
    AddressInfo() : ai(nullptr) { }
    AddressInfo(Url url) : AddressInfo(url.node(), url.service()) { }
    AddressInfo(const std::string &node, const std::string &service);
    AddressInfo(const AddressInfo &other) = delete;
    AddressInfo(AddressInfo &&other) noexcept;
    ~AddressInfo();

    const struct addrinfo *getAddressInfo() const { return ai; }

    AddressInfoIterator begin() { return AddressInfoIterator(ai); }
    AddressInfoIterator end() { return AddressInfoIterator(); }
    AddressInfoConstIterator begin() const { return cbegin(); }
    AddressInfoConstIterator end() const { return cend(); }
    AddressInfoConstIterator cbegin() const { return AddressInfoConstIterator(ai); }
    AddressInfoConstIterator cend() const { return AddressInfoConstIterator(); }

    std::ostream &debugInfo(std::ostream &os);

private:
    struct addrinfo *ai;
};

} // namespace cld

#endif

