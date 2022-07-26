#include "frp-cpp/src/TCPServer.hpp"
#include "frp-cpp/src/TCPClient.hpp"
#include <string>
#include <thread>
#include <iostream>

using namespace FRP;
using namespace std;

const size_t bufSize = 4;

const char content[bufSize + 1] = "test";

void ServerCB(std::unique_ptr<TCPConnector> connector) {
    char buf[bufSize + 1];
    bzero(buf, sizeof(buf));
    if (connector->ReadN(buf, bufSize) == -1) {
        cout<<"server CB err"<<endl;
        return;
    }
    cout<<"server get:"<<buf<<endl;
}

int main() {
    string ip = "127.0.0.1";
    uint16_t port = 3000;
    // 服务线程
    auto st = thread([&](){
        TCPServer server(ip, port);
        server.Loop(ServerCB);
    });
    st.detach();

    // 客户端
    auto ct = thread([&](){
        TCPClient client(ip, port);
        client.Connect();
        cout<<"client send:"<<content<<endl;
        client.Write((void *)content, bufSize);
    });
    ct.join();
}