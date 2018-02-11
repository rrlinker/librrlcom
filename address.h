#pragma once

#include <array>
#include <cstdint>
#include <experimental/filesystem>
#include <variant>

namespace rrl {

    class Address {
    public:
        struct IP {
            std::array<uint8_t, 16> address;
            uint16_t port;
        };
        using Path = std::experimental::filesystem::path;

        Address() = default;
        Address(IP ip)
            : value_(ip)
        {}
        Address(Path path)
            : value_(path)
        {}

        constexpr std::array<uint8_t, 16> const& address() const { return std::get<IP>(value_).address; }
        constexpr uint16_t port() const { return std::get<IP>(value_).port; }
        constexpr Path const& path() const { return std::get<Path>(value_); }

    private:
        std::variant<IP, Path> value_;
    };

}
