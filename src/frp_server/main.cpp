#include "frp-cpp/src/frp_server/FRPServer.hpp"
#include <string>
#include <iostream>

using namespace FRP;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cout<<"参数不足，请输入IP、Port"<<endl;
        return -1;
    }
    string ip(argv[1]);
    uint16_t port = atoi(argv[2]);
    FRPServer s(ip, port);
    s.Loop();
}