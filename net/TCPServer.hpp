#ifndef FRP_NET_TCPSERVER_H
#define FRP_NET_TCPSERVER_H
/*
    tcp服务器类
    服务端用来对外监听端口
    对外提供阻塞方法accept, 有新链接则暴露给业务逻辑处理类
*/

#include "frp-cpp/base/nocopyable.h"
#include <netinet/in.h>
#include <string>
#include <string.h>

namespace FRP 
{

// 不对外提供copy方法，防止有问题 
// 只在单线程使用
class TCPServer : noncopyable {
public:
    
    TCPServer(const std::string& ip, const uint16_t& port);

    int WaitConnect();

private:
    int listen();

private:
    sockaddr_in servaddr;
    std::string ip;
    uint16_t port;
    int listenFD;
};
}

#endif // FRP_NET_SERVER_H