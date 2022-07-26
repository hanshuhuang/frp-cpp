#ifndef FRP_CLIENT_H
#define FRP_CLIENT_H

#include "frp-cpp/src/pb/message.pb.h"
#include "frp-cpp/src/TCPClient.hpp"

namespace FRP
{

class FRPClient {
public:
    FRPClient(const frp::ServerConf &, const frp::LocalConf &);

    // 登陆
    int Login();

    // 等待公网服务器的命令
    void WaitLoop();

    // todo: 加入心跳机制

private:
    // 发起链接到服务器，然后回包, 需要带上链接ID
    // todo: 超时机制
    void AddConn();

private:
    // 与公网服务器的链接
    std::shared_ptr<TCPClient> managerLink;
    // 分配的客户端ID，为0则未分配
    std::string clientID;
    // 服务端配置
    frp::ServerConf serverConf;
    // 本地配置
    frp::LocalConf localConf;
};


}

#endif