#ifndef FRP_NETWORK_H
#define FRP_NETWORK_H

#include <netinet/in.h>

namespace FRP
{

class NetWorkOper {
public:
    NetWorkOper() = default;

    virtual ssize_t ReadN(void *buf, size_t count) = 0;

    virtual ssize_t Write(void *buf, size_t count) = 0;

    virtual void Close() = 0;

    // 析构函数可以是纯虚的，但纯虚析构函数必须有定义体，因为析构函数的调用是在子类中隐含的
    virtual ~NetWorkOper() {};
};

} // namespace FRP


#endif