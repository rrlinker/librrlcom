#pragma once

#include "connection.h"
#include "token.h"

#include <vector>
#include <optional>
#include <cstdint>

#pragma pack(push, 1)

namespace rrl {

    enum class MessageType : uint64_t {
#define X(TYPE, VALUE, _) TYPE = VALUE,
#include "message_definitions.h"
#undef X
    };

    struct MessageHeader {
        uint64_t size;
        MessageType type;
        MessageHeader(uint64_t size, MessageType type) noexcept
            : size(size)
            , type(type)
        {}
    };

    template<typename Body>
    struct MessageWrapper : public Body::value_type {
    private:
        MessageHeader header;
    public:
        uint64_t size() const { return header.size; }
        MessageType type() const { return header.type; }
        typename Body::value_type& body() { return *this; }
        typename Body::value_type const& body() const { return *this; }

        MessageWrapper(uint64_t size, MessageType type) noexcept
            : header(size, type)
        {}

        MessageWrapper(uint64_t size, MessageType type, typename Body::value_type const &value)
            : header(size, type)
            , Body::value_type(value)
        {}

        MessageWrapper(uint64_t size, MessageType type, typename Body::value_type &&value)
            : header(size, type)
            , Body::value_type(std::forward<typename Body::value_type>(value))
        {}

        void write(Connection &conn) const {
            conn << header.size << header.type;
            Body::write(conn, body());
        }

        void read(Connection &conn) {
            conn >> header.size >> header.type;
            Body::read(conn, header, body());
        }
    };

    namespace msg {

        namespace body {
            struct Any {
                using value_type = std::vector<std::byte>;
                static void write(Connection &conn, value_type const &value) {
                    conn.send(value.data(), value.size());
                }
                static void read(Connection &conn, MessageHeader const &header, value_type &value) {
                    value.resize(header.size);
                    conn.recv(value.data(), value.size());
                }
            };

            struct Empty {
                struct value_type {};
                static void write(Connection&, value_type const&) {}
                static void read(Connection&, value_type&) {}
            };

            template<typename T>
            struct Value {
                struct value_type { T value; };
                static void write(Connection &conn, value_type const &value) {
                    conn.send(reinterpret_cast<std::byte const*>(&value.value), sizeof(value.value));
                }
                static void read(Connection &conn, value_type &value) {
                    conn.recv(reinterpret_cast<std::byte*>(&value.value), sizeof(value.value));
                }
            };

            struct Token {
                using value_type = rrl::Token;
                static void write(Connection &conn, value_type const &value) {
                    conn.send(value.data(), value.size());
                }
                static void read(Connection &conn, value_type &value) {
                    conn.recv(value.data(), value.size());
                }
            };

            struct LinkLibrary {
                using value_type = LinkLibrary;
                static void write(Connection &conn, value_type const &value) {
                    conn.send(reinterpret_cast<std::byte const*>(value.name), sizeof(value.name));
                }
                static void read(Connection &conn, value_type &value) {
                    conn.recv(reinterpret_cast<std::byte*>(value.name), sizeof(value.name));
                }
                char name[64];
            };
        }

        struct Any : MessageWrapper<body::Any> {
            Any() : MessageWrapper(0, MessageType::Unknown) {}
            template<typename T>
            T const& cast() const { return *reinterpret_cast<T const*>(body().data()); }
        };

#define BEGIN_DEFINE_MESSAGE(TYPE, BASE) \
struct TYPE : MessageWrapper<body::BASE> { \
TYPE() : MessageWrapper(sizeof(TYPE), MessageType::TYPE) {}
#define END_DEFINE_MESSAGE() };

#define DEFINE_MESSAGE(TYPE, BASE) \
BEGIN_DEFINE_MESSAGE(TYPE, BASE) \
END_DEFINE_MESSAGE()

#define X(TYPE, _, BASE) DEFINE_MESSAGE(TYPE, BASE)
#include "message_definitions.h"
#undef X

#undef DEFINE_MESSAGE
#undef END_DEFINE_MESSAGE
#undef BEGIN_DEFINE_MESSAGE

    }


    class UnexpectedResponse : public std::runtime_error {
    public:
        UnexpectedResponse(msg::Any &&got, MessageType expected)
            : runtime_error("unexpected response")
            , got(std::move(got))
            , expected(expected)
        {}
        msg::Any got;
        MessageType expected;
    };

}

#pragma pack(pop)
