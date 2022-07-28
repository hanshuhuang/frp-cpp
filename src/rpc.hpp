#ifndef FRP_RPC_H
#define FRP_RPC_H

#include "frp-cpp/src/pb/message.pb.h"
#include "frp-cpp/src/network.hpp"
#include <tuple>
#include <memory>

namespace FRP
{
class RPC {
public:
    std::tuple<std::unique_ptr<frp::Msg>, int32_t> Call(
        frp::Msg msg, std::shared_ptr<NetWorkOper> oper);
};
}

#endif