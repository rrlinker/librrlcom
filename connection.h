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

        template<typename T>
        Connection& operator<<(T const &value) {
            send(reinterpret_cast<std::byte const*>(&value), sizeof(T));
            return *this;
        }

        template<>
        Connection& operator<<(std::string const &value) {
            uint64_t size = value.size();
            send(reinterpret_cast<std::byte const*>(&size), sizeof(size));
            send(reinterpret_cast<std::byte const*>(value.data()), size);
            return *this;
        }

        template<typename T>
        Connection& operator>>(T &value) {
            recv(reinterpret_cast<std::byte*>(&value), sizeof(T));
            return *this;
        }

        template<>
        Connection& operator>>(std::string &value) {
            uint64_t size = 0;
            recv(reinterpret_cast<std::byte*>(&size), sizeof(size));
            value.resize(size);
            recv(reinterpret_cast<std::byte*>(value.data()), size);
            return *this;
        }

    protected:
        int socket_;
    };

}
