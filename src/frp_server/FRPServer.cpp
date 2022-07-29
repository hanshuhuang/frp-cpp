#include "frp-cpp/src/frp_server/FRPServer.hpp"
#include "frp-cpp/src/idgenerator.hpp"
#include "frp-cpp/src/rpc.hpp"
#include "frp-cpp/src/pb/message.pb.h"
#include "frp-cpp/src/frp_server/LinkManager.hpp"
#include "frp-cpp/src/forward.hpp"

using namespace FRP;
using namespace frp;
using namespace std;

FRPServer::FRPServer(string ip, uint16_t port) {
    this->ip = ip;
    this->port = port;
}

void FRPServer::Handle(std::unique_ptr<TCPConnector> connector) {
    shared_ptr<TCPConnector> c = move(connector);
    RPC rpc;
    unique_ptr<Msg> msg;
    int32_t retcode = 0;
    tie(msg, retcode) = rpc.Recv(c);
    if (retcode != 0) {
        return;
    }

    switch (msg->type())
    {
    case MSGTYPE_LOGIN_REQ:
    {
        // 分配ID
        string clientID = IDGenerator::GetClientID();
        auto rsp = make_unique<frp::Msg>(Msg());
        rsp->mutable_loginrsp()->mutable_client_id()->append(clientID);
        if (rpc.Send(rsp, c) != 0) {
            cout<<"服务端登陆回包失败"<<endl;
            return;
        }
        cout<<"分配客户端ID:"<<clientID<<endl;

        // 开始proxy监听
        // 如果监听到新链接, 分配connid，通过该链接告诉客户端AddConn
        // 客户端收到AddConn，会发起新链接，Handle函数调用走到AddConn逻辑
        // 服务端把AddConn链接挂到map里，回包ok
        // 客户端收到AddConn回包，也回包ok
        // 监听链接线程尝试获取链接，开始转发
        // 开放本机端口
        shared_ptr<TCPServer> proxy = make_shared<TCPServer>(ip, 
            msg->loginreq().localconf().port());
            //void (*CallBack)(std::unique_ptr<TCPConnector> connector)
        proxy->Loop([c = c](std::unique_ptr<TCPConnector> connector){
            // 开始等待用户链接，并进行转发
            // 构造AddConn请求
            string connID = IDGenerator::GetConnID();
            shared_ptr<Msg> req = make_shared<Msg>(Msg());
            req->mutable_addconnreq()->mutable_client_id()->append(connID);
            req->set_type(MSGTYPE_ADD_CONN_REQ);

            // 发包
            RPC rpc;
            unique_ptr<Msg> rsp;
            int32_t ret = 0;
            tie(rsp, ret) = rpc.Call(req, c);
            if (ret != 0) {
                cout<<"服务端尝试新增链接网络失败"<<rsp->DebugString()<<endl;
                return;
            }

            if (rsp->addconnrsp().ret_code() != 0) {
                cout<<"服务端尝试新增链接逻辑失败"<<rsp->DebugString()<<endl;
                return;
            }

            // 尝试根据connID获取链接
            auto addConn = LinkManager::Pop(connID);
            if (addConn == nullptr) {
                cout<<"根据ID获取链接失败"<<endl;
                return;
            }

            // 开启转发
            Forward f;
            f.AsyncForward(connector, addConn);
            cout<<"客户端ID:"<<clientID<<" 链接ID:"<<connID<<" 开始转发"<<endl;
        });
        
        break;
    }
    case MSGTYPE_ADD_CONN_REQ:
    {
        unique_ptr<Msg> req = make_unique<Msg>(Msg());
        int32_t ret = 0;
        RPC rpc;
        tie(req, ret) = rpc.Recv(c);
        if (ret != 0) {
            cout<<"接受添加链接请求失败"<<endl;
            return;
        }

        LinkManager::Add(req->addconnreq().conn_id(), c);

        auto rsp = make_unique<Msg>(Msg());
        rsp->mutable_addconnrsp()->set_ret_code(0);
        if (rpc.Send(rsp, c) != 0) {
            cout<<"服务端添加链接回包失败"<<endl;
        }
        cout<<"服务端添加新链接成功 链接ID:"<<req->addconnreq().conn_id()<<endl;
        break;
    }
    default:
        break;
    }
}

void FRPServer::Loop() {
    auto svr = make_unique<TCPServer>(TCPServer(ip, port));
    svr->Loop(Handle);
}