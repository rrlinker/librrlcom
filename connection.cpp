#include "connection.h"

using namespace rrl;

Connection::Connection(int sockfd)
    : socket_(sockfd)
{}

Connection::~Connection() {}

