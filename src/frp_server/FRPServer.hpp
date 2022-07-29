#ifndef FRP_SERVER_H
#define FRP_SERVER_H

#include <string>
#include <memory>
#include "frp-cpp/src/pb/message.pb.h"
#include "frp-cpp/src/TCPServer.hpp"

namespace FRP
{
/*
    监听manager端口，处理Login和AddConn请求
    Login:
    1. 监听到新链接，则分配clientID
    2. 回包成功
        问题：
        这里好像有问题，如果监听了，此时有链接过来，但是回包还没回完，就会串数据, 所以要先回包再监听
        如果回包了但是监听失败，则直接close链接
    3. 开始监听, 并传递managerLink给该listen(注意这里不能并发处理，导致串包)
    4. 公网服务器需要存储监听到的管理端链接

    AddConn:
    1. 公网服务器监听到携带clientID、connID的请求
    2. 根据clientID拿到对应的Manager实例，然后传入connID和链接，完成add
    3. manager实例拿到内网服务器链接，设置connID对应的链接，回包AddConnRsp成功
    4. 内网服务器收到AddConnRsp后，向原来的公网服务器链接回AddConnRsp报文
    5. 公网服务器尝试用connID获取链接，获取成功则开始转发
    这里要有一个单例模式存储connID和链接到映射
    还要一个id全局生成器
*/
class FRPServer {
public:
    FRPServer(std::string ip, uint16_t port);

    void Loop();

private:
    void Handle(std::unique_ptr<TCPConnector> connector);

private:
    std::string ip;
    uint16_t port;
    std::unique_ptr<TCPServer> server;
};
    
} // namespace FRP


#endif