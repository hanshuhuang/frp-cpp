#ifndef FRP_FFORWARD_H
#define FRP_FFORWARD_H

#include "frp-cpp/src/nocopyable.h"
#include "frp-cpp/src/network.hpp"
#include <memory>

namespace FRP
{

class Forward : public noncopyable {
public:
    void AsyncForward(std::shared_ptr<NetWorkOper> nw1, std::shared_ptr<NetWorkOper> nw2);

    void Copy(std::shared_ptr<NetWorkOper> src, std::shared_ptr<NetWorkOper> dst);
};

}

#endif