#pragma once

#include "connection.h"
#include "bound_check.h"

#include <vector>
#include <variant>
#include <unordered_map>
#include <cstdint>

namespace rrl {

    enum class MessageType : uint64_t {
#define X(TYPE, VALUE) TYPE = VALUE,
#include "message_definitions.h"
#undef X
    };

    class UnexpectedMessageType : public std::runtime_error {
    public:
        UnexpectedMessageType(MessageType got, MessageType expected)
            : runtime_error("unexpected message type")
            , got(got)
            , expected(expected)
        {}
        MessageType const got;
        MessageType const expected;
    };

    struct MessageHeader {
        MessageType type;
        MessageHeader(MessageType type) noexcept
            : type(type)
        {}
    };

    template<typename Body>
    struct MessageWrapper : public Body::value_type {
    private:
        MessageHeader header;
    public:
        MessageType type() const { return header.type; }
        typename Body::value_type& body() { return *this; }
        typename Body::value_type const& body() const { return *this; }

        explicit MessageWrapper(MessageType type) noexcept
            : header(type)
        {}

        MessageWrapper(MessageType type, typename Body::value_type const &value)
            : header(type)
            , Body::value_type(value)
        {}

        MessageWrapper(MessageType type, typename Body::value_type &&value)
            : header(type)
            , Body::value_type(std::forward<typename Body::value_type>(value))
        {}

        void write_header(Connection &conn) const { conn << header.type; }
        void write_body(Connection &conn) const { Body::write(conn, body()); }
        void write(Connection &conn) const {
            write_header(conn);
            write_body(conn);
        }

        void read_header(Connection &conn) {
            if (type() == MessageType::Unknown) {
                conn >> header.type;
            } else {
                MessageType incoming_type;
                conn >> incoming_type;
                if (type() != incoming_type) {
                    throw UnexpectedMessageType(incoming_type, type());
                }
            }
        }
        void read_body(Connection &conn) { Body::read(conn, body()); }
        void read(Connection &conn) {
            read_header(conn);
            read_body(conn);
        }
    };

    namespace msg {

        namespace body {
            struct Empty {
                struct value_type {};
                static void write(Connection&, value_type const&) {}
                static void read(Connection&, value_type&) {}
            };

            // Templates
            template<typename T>
            struct Value {
                struct value_type { T value; };
                static void write(Connection &conn, value_type const &value) { conn.send(reinterpret_cast<std::byte const*>(&value.value), sizeof(value.value)); }
                static void read(Connection &conn, value_type &value) { conn.recv(reinterpret_cast<std::byte*>(&value.value), sizeof(value.value)); }
            };
            struct String {
                using value_type = std::string;
                static void write(Connection &conn, value_type const &value) { conn << value; }
                static void read(Connection &conn, value_type &value) { conn >> value; }
            };

            // Common
            struct Unknown : Empty {};
            struct OK : Empty {};

            // client <-> svcreqhandler
            struct Version : Value<uint64_t> {};
            struct LinkLibrary : String {};

            // client <-> svclinker
            struct ResolveExternalSymbol {
                struct value_type {
                    std::string library;
                    std::string symbol;
                };
                static void write(Connection &conn, value_type const &value) { conn << value.library << value.symbol; }
                static void read(Connection &conn, value_type &value) { conn >> value.library >> value.symbol; }
            };
            struct ResolvedSymbol : Value<uint64_t> {};
            struct ExportSymbol {
                struct value_type {
                    std::string symbol;
                    uint64_t address;
                };
                static void write(Connection &conn, value_type const &value) { conn << value.symbol << value.address; }
                static void read(Connection &conn, value_type &value) { conn >> value.symbol >> value.address; }
            };
            struct ReserveMemorySpace : Value<uint64_t> {};
            struct ReservedMemory : Value<uint64_t> {};
            struct CommitMemory {
                struct value_type {
                    uint64_t address;
                    uint32_t protection;
                    std::vector<std::byte> memory;
                };
                static void write(Connection &conn, value_type const &value) {
                    conn << value.address << value.protection << static_cast<uint64_t>(value.memory.size());
                    conn.send(value.memory.data(), value.memory.size());
                }
                static void read(Connection &conn, value_type &value) {
                    uint64_t memory_size;
                    conn >> value.address >> value.protection >> memory_size;
                    verify_size_bounds(memory_size);
                    value.memory.resize(static_cast<size_t>(memory_size));
                    conn.recv(value.memory.data(), value.memory.size());
                }
            };
            struct Execute : Value<uint64_t> {};

            // svclinker <-> svcsymres
            struct GetSymbolLibrary : String {};
            struct ResolvedSymbolLibrary : String {};
        }

        // Define messages
#define X(TYPE, _) \
struct TYPE : MessageWrapper<body::TYPE> { \
TYPE() : MessageWrapper(MessageType::TYPE) {} \
TYPE(body::TYPE::value_type const &bdy) : MessageWrapper(MessageType::TYPE, bdy) {} \
};
#include "message_definitions.h"
#undef X

        struct __Dummy : Unknown {};

        struct AnyHolder {
            using value_type = std::variant<
#define X(TYPE, _) TYPE,
#include "message_definitions.h"
#undef X
                __Dummy
            >;
        };

        struct Any : MessageWrapper<AnyHolder> {
            Any() : MessageWrapper(MessageType::Unknown) {}

            template<typename T>
            T& cast() { return std::get<T>(body()); }

            template<typename T>
            T const& cast() const { return std::get<T>(body()); }

            void write(Connection &conn) const {
                std::visit([&conn](auto x) {
                    x.write(conn);
                }, body());
            }

            void read(Connection &conn) {
                read_header(conn);
                body() = message_map_.at(type());
                std::visit([&conn](auto x) {
                    x.read_body(conn);
                }, body());
            }

        private:
            static inline std::unordered_map<MessageType, AnyHolder::value_type> const message_map_{
#define X(TYPE, _) { MessageType::TYPE, TYPE{} },
#include "message_definitions.h"
#undef X
            };

        };

    }

    class UnexpectedResponse : public std::runtime_error {
    public:
        UnexpectedResponse(msg::Any &&got, MessageType expected)
            : runtime_error("unexpected response")
            , got(std::move(got))
            , expected(expected)
        {}
        msg::Any const got;
        MessageType const expected;
    };

}

