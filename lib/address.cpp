#include <rrlinker/com/address.hpp>

using namespace rrl;

Address::Address() {}

Address::Address(IP ip)
    : value_(ip)
{}

Address::Address(Path path)
    : value_(path)
{}
