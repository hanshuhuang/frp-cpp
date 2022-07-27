#ifndef FRP_TCP_CLIENT_H
#define FRP_TCP_CLIENT_H

#include <string>
#include <memory>
#include "frp-cpp/src/TCPConnector.hpp"
#include "frp-cpp/src/network.hpp"

namespace FRP
{

class TCPClient : public NetWorkOper {
public:
    TCPClient(const std::string& ip, uint16_t port);

    int Connect();

    ssize_t ReadN(void *buf, size_t count);

    ssize_t Write(void *buf, size_t count);

    void Close();

    virtual ~TCPClient();

private:
    std::string ip;
    uint64_t port;
    std::unique_ptr<TCPConnector> c;
};
}
#endif