/*
    1. 先FRPClient登陆到Mock服务器
    2. 登陆完毕后，FRPClient进入waitLoop状态
    3. 服务器向login长链接发送AddConn请求，等待握手
    4. Client收到请求，向服务端发起AddConnReq
    5. 服务器根据透传的ConnID，设置future变量
    6. 服务端返回握手成功，客户端AddConn函数成功
    7. 客户端返回AddConnRsp给服务端
    8. 服务端收到请求后，尝试从future中获取sockfd
    9. 服务端开始转发(这一步测试可以先不写)
*/
#include "frp-cpp/src/TCPServer.hpp"
#include "frp-cpp/src/frp_client/FRPClient.hpp"
#include "frp-cpp/src/TCPClient.hpp"
#include "frp-cpp/src/pb/message.pb.h"
#include "frp-cpp/src/frame.hpp"
#include "frp-cpp/src/rpc.hpp"
#include <thread>
#include <iostream>
#include <unistd.h>

using namespace FRP;
using namespace frp;
using namespace std;

const string echoIP = "127.0.0.1";
const uint16_t echoPort = 3000;
const int buffSize = 4096;
const string connID = "connID_test";
const string clientID = "clientID_test";
const string mockSvrIP = "127.0.0.1";
const uint16_t mockSvrPort = 3001;

void echo(std::unique_ptr<TCPConnector> connector) {
    cout<<"echo服务收到链接"<<endl;
    char buff[buffSize];
    auto rret = connector->Read((void*)buff, buffSize - 1);
    if (rret <= 0) {
        cout<<"echoSvr Read异常返回"<<endl;
        return;
    }

    buff[rret] = 0;
    cout<<"echoSvr recv:"<<buff<<endl;
}

shared_ptr<TCPConnector> loginConnector = nullptr;
shared_ptr<TCPConnector> addConnConnector = nullptr;
void mockHandle(std::unique_ptr<TCPConnector> connector) {
    Framer f;
    shared_ptr<TCPConnector> sc = move(connector);
    auto reqData = f.GetFrame(sc);
    Msg req;
    req.ParseFromString(reqData);
    cout<<"mock服务器收到请求:"<<req.DebugString()<<endl;

    switch (req.type())
    {
    case MSGTYPE_LOGIN_REQ:
    {
        // 分配ConnID
        shared_ptr<Msg> msg = make_shared<Msg>(Msg());
        msg->set_type(MSGTYPE_LOGIN_RSP);
        msg->mutable_loginrsp()->mutable_client_id()->append(clientID);
        msg->mutable_loginrsp()->set_ret_code(0);
        Framer f;
        string rspData;
        int32_t retcode = 0;
        cout<<"登陆回包"<<msg->DebugString()<<endl;
        tie(rspData, retcode) = f.WriteFrame(msg);
        if (retcode != 0) {
            cout<<"写login帧失败"<<endl;
            return;
        } 

        retcode = sc->Write((void*)rspData.c_str(), rspData.size());
        if (retcode <= 0) {
            cout<<"mock服务器登陆请求回包失败"<<endl;
            return;
        }
        loginConnector = sc;
        break;
    }
    case MSGTYPE_ADD_CONN_REQ:
    {
       // 回包成功
        auto rsp = make_shared<Msg>(Msg());
        rsp->mutable_addconnrsp()->mutable_conn_id()->append(connID);
        string rspData;
        int32_t retcode = 0;
        tie(rspData, retcode) = Framer().WriteFrame(rsp);
        if (retcode != 0) {
            cout<<"addConn 写帧失败"<<endl;
            return;
        }
        retcode = sc->Write((void*)rspData.c_str(), rspData.size());
        if (retcode <= 0) {
            cout<<"addConn 回包失败"<<endl;
            return;
        }
        addConnConnector = sc;
        cout<<"mock服务器addConn回包成功"<<endl;
        break;
    }
    default:
        cout<<"mock服务器未知类型"<<endl;
        break;
    }
}

int main() {
    // echo svr
    thread([=](){
        unique_ptr<TCPServer> echoSvr = make_unique<TCPServer>(
            TCPServer(echoIP, echoPort));
        echoSvr->Loop(echo);
    }).detach();

    // Mock服务器
    // 监听链接，收到后发起AddConn请求
    thread([=](){
        unique_ptr<TCPServer> mockSvr = make_unique<TCPServer>(
            TCPServer(mockSvrIP, mockSvrPort));
        mockSvr->Loop(mockHandle);
    }).detach();

    // 等待服务准备好
    sleep(1);

    // FRP客户端登陆, 以及waitLoop
    thread([=](){
        ServerConf sf;
        sf.mutable_ip()->append(mockSvrIP);
        sf.set_port(mockSvrPort);

        LocalConf lf;
        lf.mutable_ip()->append(echoIP);
        lf.set_port(echoPort);

        auto frpCli = make_shared<FRPClient>(FRPClient(sf, lf));
        if (frpCli->Login() != 0) {
            cout<<"登陆失败"<<endl;
            return;
        }
        cout<<"登陆成功, 进入waitLoop"<<endl;
        frpCli->WaitLoop();
    }).detach();
    
    // 主动往客户端发起AddConn请求
    // 等待登陆成功
    cout<<"等待登陆"<<endl;
    while (loginConnector == nullptr) {
        sleep(1);
    }

    // 模拟服务端写
    RPC rpc;
    auto addConnReq = make_unique<Msg>(Msg());
    addConnReq->set_type(MSGTYPE_ADD_CONN_REQ);
    addConnReq->mutable_addconnreq()->mutable_conn_id()->append(connID);
    unique_ptr<Msg> rsp;
    int32_t ret;
    tie(rsp, ret) = rpc.Call(move(addConnReq), loginConnector);
    if (ret != 0) {
        cout<<"mock服务发送添加链接请求失败"<<endl;
        return 0;
    }

    // 判断回包
    if (rsp->addconnrsp().ret_code() != 0) {
        cout<<"addConn客户端回包失败 "<<rsp->addconnrsp().err_msg()<<endl;
        return 0;
    }

    // 往新链接写数据查看是否可用
    cout<<"尝试写入数据到新链接"<<endl;
    string mockData = "test add conn echoSvr";
    if (addConnConnector->Write((void*)mockData.c_str(), mockData.size()) <= 0) {
        cout<<"测试写链接失败"<<endl;
        return 0;
    }
    cout<<"完成数据写入"<<endl;
    sleep(3);
}