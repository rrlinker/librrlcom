#pragma once

#include <iostream>
#include <string>
#include <cstddef>

#include "address.h"

#include <system_error>

namespace rrl {

    class Connection {
    public:
        explicit Connection(int fd = -1);
        virtual ~Connection() = 0;

        virtual void connect(Address const &address) = 0;
        virtual void disconnect() = 0;

        virtual void send(std::byte const *data, uint64_t length) = 0;
        virtual void recv(std::byte *data, uint64_t length) = 0;

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

