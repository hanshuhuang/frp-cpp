#ifndef FRP_NETWORK_H
#define FRP_NETWORK_H

#include <netinet/in.h>

namespace FRP
{

class NetWorkOper {
public:
    virtual ssize_t ReadN(void *buf, size_t count);

    virtual ssize_t Write(void *buf, size_t count);

    virtual void Close();

    virtual ~NetWorkOper();
};

} // namespace FRP


#endif