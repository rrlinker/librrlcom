#pragma once

#include <iostream>
#include <string>

#include "address.h"

#include <system_error>

namespace rrl {

    class Connection {
    public:
        explicit Connection(int fd = -1);

        virtual void connect(Address const &address) = 0;
        virtual void disconnect() = 0;

        virtual void send(std::byte const *data, size_t length) = 0;
        virtual void recv(std::byte *data, size_t length) = 0;

        virtual void send(std::string const &str);
        virtual void recv(std::string &str);

        template<typename T>
        Connection& operator<<(T const &value) {
            send(reinterpret_cast<std::byte const*>(&value), sizeof(T));
            return *this;
        }

        template<typename T>
        Connection& operator<<(T &&value) {
            send(reinterpret_cast<std::byte const*>(&value), sizeof(T));
            return *this;
        }

        template<typename T>
        Connection& operator>>(T &value) {
            recv(reinterpret_cast<std::byte*>(&value), sizeof(T));
            return *this;
        }

    protected:
        int socket_;
    };

}

