#ifndef FRP_RPC_H
#define FRP_RPC_H

#include "frp-cpp/src/pb/message.pb.h"
#include <tuple>

namespace FRP
{
class RPC {
public:
    std::tuple<std::unique_ptr<frp::Msg>, int32_t> Call(frp::Msg msg);
};
}

#endif