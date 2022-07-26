#ifndef FRP_CONNECTOR_H
#define FRP_CONNECTOR_H

#include <string>
#include <netinet/in.h>
#include "frp-cpp/src/network.hpp"

namespace FRP
{
/*
    管理已经存在的链接
*/
class TCPConnector : public NetWorkOper {
public:
    TCPConnector(int sockfd);

    TCPConnector(TCPConnector&&);

    ssize_t ReadN(void *buf, size_t count);

    ssize_t Write(void *buf, size_t count);

    ssize_t Read(void *buf, size_t count);

    void Close();

    virtual ~TCPConnector();

private:
    int sockfd;
};
}
#endif