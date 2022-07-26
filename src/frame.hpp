#ifndef FRP_FRAME_H
#define FRP_FEAME_H

#include <string>
#include <memory>
#include "frp-cpp/src/network.hpp"

namespace FRP {

class Framer {
public:
    std::string GetFrame(shared_ptr<NetWorkOper> net);
};

}

#endif