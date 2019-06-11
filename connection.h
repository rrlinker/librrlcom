#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <cstddef>

#include "address.h"
#include "bound_check.h"

#include <system_error>

namespace rrl {

    class Connection {
    public:
        explicit Connection(intptr_t fd = -1)
            : socket_(fd)
        {}
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

        template<typename T>
        Connection& operator>>(T &value) {
            recv(reinterpret_cast<std::byte*>(&value), sizeof(T));
            return *this;
        }

        template<typename T>
        Connection& operator<<(std::vector<T> const &value) {
            uint64_t size = value.size();
            send(reinterpret_cast<std::byte const*>(&size), sizeof(size));
            send(value.data(), value.size() * sizeof(T));
            return *this;
        }

        template<typename T>
        Connection& operator>>(std::vector<T> &value) {
            uint64_t size = 0;
            recv(reinterpret_cast<std::byte*>(&size), sizeof(size));
            verify_size_bounds(size);
            value.resize(static_cast<size_t>(size));
            recv(reinterpret_cast<std::byte*>(value.data()), size);
            return *this;
        }

    protected:
        intptr_t socket_;
    };

    template<>
    inline Connection& Connection::operator<<(std::string const &value) {
        uint64_t size = value.size();
        send(reinterpret_cast<std::byte const*>(&size), sizeof(size));
        send(reinterpret_cast<std::byte const*>(value.data()), size);
        return *this;
    }

    template<>
    inline Connection& Connection::operator>>(std::string &value) {
        uint64_t size = 0;
        recv(reinterpret_cast<std::byte*>(&size), sizeof(size));
        verify_size_bounds(size);
        value.resize(static_cast<size_t>(size));
        recv(reinterpret_cast<std::byte*>(value.data()), size);
        return *this;
    }

    inline Connection::~Connection() {}

}
