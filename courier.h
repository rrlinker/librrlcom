#pragma once

#include "message.h"
#include "connection.h"

namespace rrl {

    class Courier {
    public:
        virtual ~Courier() {}
        virtual msg::Any receive() = 0;
        virtual void send(msg::Any const &msg) = 0;
    };

}
