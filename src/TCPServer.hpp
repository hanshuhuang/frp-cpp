#ifndef FRP_NET_TCPSERVER_H
#define FRP_NET_TCPSERVER_H
/*
    tcp服务器类
    服务端用来对外监听端口
    对外提供阻塞方法accept, 有新链接则暴露给业务逻辑处理类
*/

#include "frp-cpp/src/nocopyable.h"
#include "frp-cpp/src/TCPConnector.hpp"
#include <netinet/in.h>
#include <string>
#include <string.h>
#include <memory>
#include <functional>

namespace FRP 
{
typedef void (*CallBack)(std::unique_ptr<TCPConnector> connector);
// 不对外提供copy方法，防止有问题 
// 只在单线程使用
class TCPServer : noncopyable {
public:
    
    TCPServer(const std::string& ip, const uint16_t& port);

    TCPServer(FRP::TCPServer&&);
    
    /*
        使用场景：
        1. 监听外网服务器管理端口，对新的客户端链接查询，使用一收一发模型进行监听
        2. 用户发起链接，使用管理链接发送新增链接请求
    */
   // typedef void (*CallBack)(std::unique_ptr<TCPConnector> connector);
    void Loop(std::function<void (std::unique_ptr<TCPConnector> connector)> callback);

private:
    int Listen();

private:
    sockaddr_in servaddr;
    std::string ip;
    uint16_t port;
    int listenFD;
};
}

#endif // FRP_NET_SERVER_H