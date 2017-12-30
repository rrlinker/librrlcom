#pragma once

#include "message.h"
#include "connection.h"

namespace rrl {

    class Courier {
    public:
        Courier(Connection &conn);

        msg::Any receive() {
            msg::Any msg;
            msg.read(conn_);
            return msg;
        }

        template<typename T>
        void send(MessageWrapper<T> const &msg) {
            msg.write(conn_);
        }

    private:
        Connection &conn_;
    };

}
