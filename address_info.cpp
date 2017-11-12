#include "address_info.h"
#include "wrapper.h"

namespace cld {

AddressInfo::AddressInfo(const std::string & node, const std::string & service)
{
    struct addrinfo hints = { };
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = IPPROTO_TCP; // Only tcp
    wrapper::GetAddressInfo(node.c_str(), service.c_str(), &hints, &ai);
}

AddressInfo::AddressInfo(AddressInfo && other) noexcept
{
    ai = other.ai;
    other.ai = nullptr;
}

AddressInfo::~AddressInfo()
{
    if (ai != nullptr)
        freeaddrinfo(ai);
}

std::ostream &AddressInfo::debugInfo(std::ostream & os)
{
    os << "[Debug] Address info:";
    for (const struct addrinfo &ai : *this) {
        os << "\n\tAddress: "
            << wrapper::InetNtop(ai.ai_family, wrapper::GetAddress(ai.ai_addr));
    }
    return os << std::endl;
}

AddressInfoConstIterator & AddressInfoConstIterator::operator++()
{
    if (ai != nullptr)
        ai = ai->ai_next;
    else
        throw std::out_of_range("AddressInfo iterator out of range");
    return *this;
}

AddressInfoConstIterator AddressInfoConstIterator::operator++(int)
{
    if (ai != nullptr)
        ai = ai->ai_next;
    else
        throw std::out_of_range("AddressInfo iterator out of range");
    return *this;
}

AddressInfoIterator & AddressInfoIterator::operator++()
{
    if (ai != nullptr)
        ai = ai->ai_next;
    else
        throw std::out_of_range("AddressInfo iterator out of range");
    return *this;
}

AddressInfoIterator AddressInfoIterator::operator++(int)
{
    if (ai != nullptr)
        ai = ai->ai_next;
    else
        throw std::out_of_range("AddressInfo iterator out of range");
    return *this;
}

} // namespace cld
