#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <variant>

namespace rrl {

    class Address {
    public:
        struct IP {
            std::array<uint8_t, 16> address;
            uint16_t port;
        };
        using Path = std::filesystem::path;

        Address();
        Address(IP ip);
        Address(Path path);

        std::array<uint8_t, 16> const& address() const { return std::get<IP>(value_).address; }
        uint16_t port() const { return std::get<IP>(value_).port; }
        Path const& path() const { return std::get<Path>(value_); }

    private:
        std::variant<IP, Path> value_;
    };

}
