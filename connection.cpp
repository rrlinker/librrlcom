#include "connection.h"

using namespace rrl;

Connection::Connection(int sockfd)
    : socket_(sockfd)
{}

Connection::~Connection() {}

template<>
Connection& Connection::operator<<(std::string const &value) {
    uint64_t size = value.size();
    send(reinterpret_cast<std::byte const*>(&size), sizeof(size));
    send(reinterpret_cast<std::byte const*>(value.data()), size);
    return *this;
}

template<>
Connection& Connection::operator>>(std::string &value) {
    uint64_t size = 0;
    recv(reinterpret_cast<std::byte*>(&size), sizeof(size));
    value.resize(size);
    recv(reinterpret_cast<std::byte*>(value.data()), size);
    return *this;
}

