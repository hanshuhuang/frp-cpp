/*
    本地端口22，远程管理端口3000，远程开放端口3001
    FRPServer开始监听
    使用FRPClient进行登陆
*/
#include "frp-cpp/src/frp_server/FRPServer.hpp"
#include "frp-cpp/src/frp_client/FRPClient.hpp"
#include "frp-cpp/src/pb/message.pb.h"
#include <string>
#include <thread>
#include <iostream>
#include <unistd.h>

using namespace FRP;
using namespace std;
using namespace frp;

const string ip = "127.0.0.1";
const uint16_t localPort = 22;
const uint16_t serverPort = 3000;
const uint16_t proxyPort = 3001;

int main() {
    thread([](){
        FRPServer s(ip, serverPort);
        s.Loop();
    }).detach();

    sleep(1);
    
    ServerConf sf;
    sf.mutable_ip()->append(ip);
    sf.set_port(serverPort);

    LocalConf lf;
    lf.mutable_ip()->append(ip);
    lf.set_port(localPort);
    lf.set_openport(proxyPort);
    FRPClient cli(sf, lf);
    if (cli.Login() != 0) {
        cout<<"登陆失败"<<endl;
        return 0;
    }

    cout<<"开始监听"<<endl;
    cli.WaitLoop();
}