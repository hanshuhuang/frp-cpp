#include "frp-cpp/src/frp_client/FRPClient.hpp"
#include "frp-cpp/src/pb/message.pb.h"
#include <string>

using namespace FRP;
using namespace frp;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cout<<"参数不足, 请输入公网服务器IP、公务服务器端口、本机端口、远端映射端口"<<endl;
        return -1;
    }

    string serverIP = string(argv[1]);
    uint16_t port = atoi(argv[2]);
    uint16_t localPort = atoi(argv[3]);
    uint16_t remotePort = atoi(argv[4]);

    ServerConf sf;
    sf.mutable_ip()->append(serverIP);
    sf.set_port(port);

    LocalConf lf;
    lf.mutable_ip()->append("127.0.0.1");
    lf.set_port(localPort);
    lf.set_openport(remotePort);

    FRPClient cli(sf, lf);
    if (cli.Login() != 0) {
        cout<<"login err"<<endl;
        return -1;
    }

    cli.WaitLoop();
}