#include "connection.h"

using namespace rrl;

Connection::Connection(intptr_t sockfd)
    : socket_(sockfd)
{}

Connection::~Connection() {}

