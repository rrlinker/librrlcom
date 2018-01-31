#pragma once

#include <array>
#include <initializer_list>
#include <cstdint>

namespace rrl {

    class Address {
    public:
        Address() = default;
        Address(std::array<uint8_t, 16> addr, uint16_t port) noexcept
            : addr_(addr)
            , port_(port)
        {}

        constexpr uint8_t const* addr() const { return addr_.data(); }
        constexpr uint16_t port() const { return port_; }

    private:
        std::array<uint8_t, 16> addr_;
        uint16_t port_;
    };

}
