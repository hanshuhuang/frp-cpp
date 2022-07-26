#ifndef FRP_RPC_H
#define FRP_RPC_H

#include "frp-cpp/src/pb/message.pb.h"

namespace FRP
{
class RPC {
public:
    tuple<unique_ptr<frp::Msg>, int32_t> Call(frp::Msg msg);
};
}

#endif