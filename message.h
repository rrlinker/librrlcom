#pragma once

#include "connection.h"
#include "token.h"

#include <vector>
#include <memory>
#include <cstdint>

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

        void read_header(Connection &conn) {
            conn >> header.size >> header.type;
        }

        void read(Connection &conn) {
            read_header(conn);
            Body::read(conn, header, body());
        }
    };

    namespace msg {

        namespace body {
            struct Any {
                using value_type = std::shared_ptr<void>;
                static void write(Connection &conn, value_type const &value) {
                    throw "cannot write body of Any";
                }
                static void read(Connection &conn, MessageHeader const &header, value_type &value) {
                    throw "cannot read body of Any";
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


            template<typename T>
            struct vector_of_values {
                using value_type = std::vector<T>;

                static void write(Connection &conn, value_type const &value) {
                    conn << (uint64_t)value.size();
                    for (auto ptr : value) {
                        conn << ptr;
                    }
                }
                static void read(Connection &conn, value_type &value) {
                    uint64_t size;
                    conn >> size;
                    value.resize(size);
                    for (auto& ptr : value) {
                        conn >> ptr;
                    }
                }
            };

            template<typename T, typename U>
            struct vector_of_value_pairs {
                using value_type = std::vector<std::pair<T, U>>;
                static void write(Connection &conn, value_type const &value) {
                    conn << (uint64_t)value.size();
                    for (auto [addr, size] : value) {
                        conn << addr << size;

                    }
                }
                static void read(Connection &conn, value_type &value) {
                    uint64_t size;
                    conn >> size;
                    value.resize(size);
                    for (auto& [addr, size] : value) {
                        conn >> addr >> size;
                    }
                }
            };
            struct LinkLibrary {
                using value_type = LinkLibrary;

                char name[64];

                static void write(Connection &conn, value_type const &value) {
                    conn.send(reinterpret_cast<std::byte const*>(value.name), sizeof(value.name));
                }
                static void read(Connection &conn, value_type &value) {
                    conn.recv(reinterpret_cast<std::byte*>(value.name), sizeof(value.name));
                }
            };

            struct ResolveExternalSymbols {
                using value_type = std::vector<std::pair<std::string, std::string>>;

                static void write(Connection &conn, value_type const &value) {
                    conn << (uint64_t)value.size();
                    for (auto const& [lib, sym] : value) {
                        conn.send(lib);
                        conn.send(sym);
                    }
                }
                static void read(Connection &conn, value_type &value) {
                    uint64_t size;
                    conn >> size;
                    value.resize(size);
                    for (auto& [lib, sym] : value) {
                        conn.recv(lib);
                        conn.recv(sym);
                    }
                }
            };

            using ResolvedSymbols = vector_of_values<uint64_t>;
            using ReserveMemorySpaces = vector_of_value_pairs<uint64_t, uint64_t>;
            using ReservedMemory = vector_of_values<uint64_t>;

            struct CommitMemory {
                using value_type = CommitMemory;

                uint64_t address;
                uint32_t protection;
                std::vector<std::byte> memory;
                static void write(Connection &conn, value_type const &value) {
                    conn << value.address << value.protection << (uint64_t)value.memory.size();
                    conn.send(value.memory.data(), value.memory.size());
                }
                static void read(Connection &conn, value_type &value) {
                    uint64_t memory_size;
                    conn >> value.address >> value.protection >> memory_size;
                    value.memory.resize(memory_size);
                    conn.recv(value.memory.data(), value.memory.size());
                }
            };
        }

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

        struct Any : MessageWrapper<body::Any> {
            Any() : MessageWrapper(0, MessageType::Unknown) {}
            template<typename T>
            T& cast() { return *static_cast<T*>(body().get()); }
            template<typename T>
            T const& cast() const { return *static_cast<T const*>(body().get()); }
            void read(Connection &conn) {
                read_header(conn);
                body() = std::make_shared<ReservedMemory>();
                switch (type()) {
#define X(TYPE, _, BODY) case MessageType::TYPE: \
body() = std::make_shared<TYPE>(); \
body::BODY::read(conn, cast<TYPE>().body()); \
break;
#include "message_definitions.h"
#undef X
                }
            }
        };

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
