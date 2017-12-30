#include "courier.h"


using namespace rrl;


Courier::Courier(Connection &conn)
    : conn_(conn)
{}

//bool Courier::receive(Message &msg) {
//    return false;
//}

//void Courier::send(const Message &msg) {
//    conn_.send(reinterpret_cast<const std::byte*>(&msg), sizeof(Message));
//}
