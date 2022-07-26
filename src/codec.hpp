#ifndef FRP_CODEC_H
#define FRP_CODEC_H

#include "frp-cpp/src/pb/message.pb.h"
#include <string>
#include <memory>
#include <iostream>

namespace FRP 
{

class Decoder {
    std::unique_ptr<frp::Msg> Decode(const std::string& buffer);
};

class Encoder {
    std::string Encode(const frp::Msg& msg);
};

}

#endif