#pragma once

#include "courier.hpp"

namespace rrl {

    class RawCourier final : public Courier {
    public:
        RawCourier(Connection &conn);
        virtual ~RawCourier();
        virtual msg::Any receive() override;
        virtual void send(msg::Any const &msg) override;
    private:
        Connection &conn_;
    };

}
