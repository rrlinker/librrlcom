#pragma once

#include <rrlinker/com/message.hpp>
#include <rrlinker/com/connection.hpp>

namespace rrl {

    class Courier {
    public:
        virtual ~Courier() noexcept(false);
        virtual msg::Any receive() = 0;
        virtual void send(msg::Any const &msg) = 0;
    };

}
