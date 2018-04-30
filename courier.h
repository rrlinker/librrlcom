#pragma once

#include "message.h"
#include "connection.h"

namespace rrl {

    class Courier {
    public:
        Courier(Connection &conn)
            : conn_(conn)
        {}

        msg::Any receive() {
            msg::Any msg;
            msg.read(conn_);
            return msg;
        }

        template<typename T>
        T receive() {
            T msg;
            msg.read(conn_);
            return msg;
        }

        template<typename T>
        void send(MessageWrapper<T> const &msg) {
            msg.write(conn_);
        }

        template<typename T, typename ...Args>
        void send(Args&&... args) {
            T msg(std::forward<Args>(args)...);
            msg.write(conn_);
        }

    private:
        Connection &conn_;
    };

}
