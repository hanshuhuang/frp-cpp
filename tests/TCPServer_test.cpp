#include "frp-cpp/src/TCPServer.hpp"
#include "frp-cpp/src/error.hpp"
#include <iostream>

using namespace std;
using namespace FRP;

// 编译 g++ TCPServer_test.cpp ../src/TCPConnector.cpp ../src/TCPServer.cpp  -I ~/Desktop/code/github/frp-cpp -lpthread

const uint32_t buffSize = 10;

void PrintHandle(unique_ptr<TCPConnector> c) {
    cout<<"start"<<endl;
    char buf[buffSize];
    while (true) {
        bzero(buf, buffSize);
        if (c->ReadN(buf, buffSize - 1) == -1) {
            cout<<"read err"<<endl;
            return;
        }
        cout<<buf<<endl;
    }
}

int main() {
    string ip = "127.0.0.1";
    uint16_t port = 3000;
    TCPServer server(ip, port);
    server.Loop(PrintHandle);
}