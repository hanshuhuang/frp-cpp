/*
    对外开放端口，ssh登陆后，
    连接虚拟机ssh端口
    两个链接开始forward
    预期现象是可以正常进行ssh到虚拟机
*/

#include "frp-cpp/src/forward.hpp"
#include "frp-cpp/src/TCPServer.hpp"
#include "frp-cpp/src/TCPClient.hpp"
#include <iostream>

using namespace FRP;
using namespace std;

void forwardLink(std::unique_ptr<TCPConnector> connector) {
    auto client = make_unique<TCPClient>(TCPClient(move("127.0.0.1"), 22));
    if (client->Connect() != 0) {
        cout<<"连接22端口失败"<<endl;
        return;
    }

    Forward f;
    f.AsyncForward(move(client), move(connector));
}

int main() {
    TCPServer server(move("127.0.0.1"), 3000);
    server.Loop(forwardLink);
}