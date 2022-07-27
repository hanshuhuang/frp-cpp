#ifndef FRP_FRAME_H
#define FRP_FEAME_H

#include <string>
#include <memory>
#include <tuple>
#include "frp-cpp/src/network.hpp"
#include "frp-cpp/src/pb/message.pb.h"

namespace FRP {

class Framer {
public:
    // todo：感觉设计得不好, 需要返回错误码退出循环
    std::string GetFrame(std::shared_ptr<NetWorkOper> net);
    std::tuple<std::string, int32_t> WriteFrame(std::shared_ptr<frp::Msg> msg);
    static uint32_t ReadUint32(char*);
    static bool checkBigEnd();
    static std::string WriteUint32(uint32_t num);
};

}

#endif