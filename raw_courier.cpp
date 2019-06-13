#include "raw_courier.hpp"

using namespace rrl;

RawCourier::RawCourier(Connection &conn)
    : conn_(conn)
{}

RawCourier::~RawCourier() {}

msg::Any RawCourier::receive() {
    msg::Any msg;
    msg.read(conn_);
    return msg;
}

void RawCourier::send(msg::Any const &msg) {
    msg.write(conn_);
}
