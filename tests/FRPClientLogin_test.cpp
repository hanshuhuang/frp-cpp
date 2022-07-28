/*
    1. 测试Login时带的参数对不对
    本机端口开放127.0.0.1,3000
    看客户端链接后的登陆报文对不对
*/
#include "frp-cpp/src/frp_client/FRPClient.hpp"
#include "frp-cpp/src/TCPServer.hpp"
#include "frp-cpp/src/frame.hpp"
#include <thread>
#include <iostream>
#include <unistd.h>

using namespace FRP;
using namespace frp;
using namespace std;

const string ip = "127.0.0.1";
const uint16_t port = 3000;

void Print(std::unique_ptr<TCPConnector> connector) {
    cout<<"收到请求"<<endl;
    shared_ptr<TCPConnector> c = move(connector);
    Framer framer;
    auto data = framer.GetFrame(c);
    auto msg = Msg();
    msg.ParseFromString(data);
    cout<<"服务端监听到登陆请求"<<msg.DebugString()<<endl;

    // 回包
    auto rsp = make_shared<Msg>(Msg());
    rsp->mutable_loginrsp()->mutable_client_id()->append("testCliID");
    string rspData;
    int32_t ret;
    tie(rspData, ret) = framer.WriteFrame(rsp);
    if (ret != 0) {
        cout<<"回包失败"<<endl;
        return;
    }
    c->Write((void*)rspData.c_str(), rspData.size());
}

int main() {
    ServerConf sConf;
    sConf.mutable_ip()->append(ip);
    sConf.set_port(port);

    LocalConf lConf;
    lConf.mutable_ip()->append("test");
    lConf.set_port(123);

    thread([&](){
        // 本机开放mock服务
        TCPServer s(ip, port);  
        s.Loop(Print);
    }).detach();
    
    sleep(1);

    FRPClient fc(sConf, lConf);
    fc.Login();
}