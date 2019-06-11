#pragma once

#include "courier.h"

namespace rrl {

    class RawCourier : public Courier {
    public:
        RawCourier(Connection &conn)
            : conn_(conn)
        {}
        virtual ~RawCourier() {}

        virtual msg::Any receive() override {
            msg::Any msg;
            msg.read(conn_);
            return msg;
        }

        virtual void send(msg::Any const &msg) override {
            msg.write(conn_);
        }

    private:
        Connection &conn_;
    };

}
