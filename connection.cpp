#include "connection.h"

using namespace rrl;

Connection::Connection(int sockfd)
    : socket_(sockfd)
{}

void Connection::send(std::string const &str) {
    size_t size = str.size();
    send(reinterpret_cast<std::byte const*>(&size), sizeof(size));
    send(reinterpret_cast<std::byte const*>(str.data()), str.size());
}

void Connection::recv(std::string &str) {
    size_t size = 0;
    recv(reinterpret_cast<std::byte*>(&size), sizeof(size));
    str.resize(size);
    recv(reinterpret_cast<std::byte*>(str.data()), str.size());
}
