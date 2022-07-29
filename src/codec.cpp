#include "frp-cpp/src/codec.hpp"
#include "frp-cpp/src/pb/message.pb.h"
#include <string>
#include <memory>
#include <iostream>

using namespace FRP;
using namespace std;

std::unique_ptr<frp::Msg> Decoder::Decode(const std::string& buffer) {
    auto msg = std::make_unique<frp::Msg>(frp::Msg());
    if (!msg->ParseFromString(buffer)) {
        return nullptr;
    }
    return msg;
}

std::string Encoder::Encode(const frp::Msg& msg) {
    std::string res;
    if (!msg.SerializeToString(&res)) {
        std::cout<<"序列化失败"<<std::endl;
    }
    return res;
}