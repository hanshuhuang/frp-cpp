#include "frp-cpp/src/pb/message.pb.h"
#include "frp-cpp/src/frp_client/FRPClient.hpp"
#include "frp-cpp/src/frame.hpp"
#include "frp-cpp/src/TCPClient.hpp"
#include "frp-cpp/src/codec.hpp"
#include "frp-cpp/src/rpc.hpp"
#include "frp-cpp/src/forward.hpp"
#include <memory>
#include <iostream>
#include <thread>

using namespace FRP;
using namespace std;
using namespace frp;

/*
    FRP客户端
    1. 启动时先解析配置文件(todo)
    2. 连接外网服务器，发送配置完成登陆流程
    3. 监听链接请求，如果服务器发送addConn请求()，则新开线程转发到本地ssh端口
    4. 回包ok，让服务端开始转发
*/
FRPClient::FRPClient(const frp::ServerConf& sc, const frp::LocalConf& lc) {
    serverConf = sc;
    localConf = lc;
}

/*
    登陆功能：
    1. 发送LoginReq，告诉服务端需要监听的端口
    2. 查看回包是否成功，否则打印日志退出
    3. for循环监听请求，进行处理
    4. 收到AddConn请求，则新起线程
    5. 在新线程尝试链接本地端口
    6. 分配链接id，并告诉服务端，让服务端用该链接转发请求
    7. 开启转发
*/
int FRPClient::Login() {
    // 连接服务端
    managerLink = make_shared<TCPClient>(
        TCPClient(serverConf.ip(), serverConf.port()));
    if (managerLink->Connect() != 0) {
        cout<<"客户端Login连接公网服务器失败"<<serverConf.DebugString()<<endl;
        return -1;
    }

    // 请求登陆
    auto msg = make_shared<Msg>(Msg());
    msg->set_type(MSGTYPE_LOGIN_REQ);
    msg->mutable_loginreq()->mutable_localconf()->CopyFrom(localConf);

    RPC rpc;
    unique_ptr<Msg> rsp;
    int32_t retcode = 0;
    tie(rsp, retcode) = rpc.Call(msg, managerLink);
    if (retcode != 0) {
        cout<<"发包失败"<<endl;
        return -1;
    }

    // 赋值
    cout<<"FRPClient Login"<<rsp->DebugString()<<endl;
    clientID = rsp->loginrsp().client_id();
    cout<<"登陆成功, 分配ClientID: "<<clientID<<endl;
    return 0;
}

/*
    等待服务端的命令
*/
void FRPClient::WaitLoop() {
    Framer framer;
    Decoder decoder;
    // todo: 链接出问题了也不会退出, 需要优化
    while (true) {
        auto data = framer.GetFrame(managerLink);
        auto msg = decoder.Decode(data);
        if (msg == nullptr) {
            cout<<"wait loop获取到空命令"<<endl;
            continue;
        }
        cout<<"客户端收到添加链接请求:"<<msg->DebugString()<<endl;

        switch (msg->type())
        {
        case MSGTYPE_ADD_CONN_REQ:
        {
            int32_t ret = 0;
            string errMsg;
            tie(ret, errMsg) = AddConn(move(msg));
            auto rsp = make_shared<Msg>(Msg());
            rsp->set_type(MSGTYPE_ADD_CONN_RSP);
            rsp->mutable_addconnrsp()->mutable_err_msg()->append(errMsg);
            rsp->mutable_addconnrsp()->set_ret_code(ret);
            RPC rpc;
            if (rpc.Send(rsp, managerLink) != 0) {
                cout<<"客户端添加请求回包失败"<<endl;
            }
            break;
        }
        default:
            cout<<"unknown command type"<<msg->DebugString()<<endl;
        }
    }
    cout<<"退出waitloop"<<endl;
}

/*
    1. 获取链接的ID
    2. 发起connect到本地，开始转发
    3. 带上linkID发起connect到server
    4. 等待server回包ok, 开始
*/
tuple<shared_ptr<TCPClient>, int32_t, string> FRPClient::addConn(const string& ip, uint16_t port) {
    // 发起链接到本地端口
    auto localConn = make_shared<TCPClient>(TCPClient(ip, port));
    if (localConn->Connect() != 0) {
        cout<<"链接端口失败"<<localConf.DebugString()<<endl;
        return {nullptr, -1, "链接端口失败"};
    }

    return {move(localConn), 0, ""};
}

tuple<int32_t, string> FRPClient::AddConn(std::unique_ptr<frp::Msg> msg) {
    // 检查链接ID
    auto connID = msg->addconnreq().conn_id();
    if (connID.empty()) {
        cout<<"链接ID为空错误"<<endl;
        return {-1, "link id empty"};
    }

    // 开启本地链接
    int32_t retCode = 0;
    string errMsg;
    shared_ptr<TCPClient> localConn;
    tie(localConn, retCode, errMsg) = addConn(localConf.ip(), localConf.port());
    if (retCode != 0) {
        return {retCode, errMsg};
    }
    cout<<"客户端开启本地链接成功"<<endl;

    // 发起链接到server
    shared_ptr<TCPClient> serverConn;
    tie(serverConn, retCode, errMsg) = addConn(serverConf.ip(), serverConf.port());
    if (retCode != 0) {
        return {retCode, errMsg};
    }
    cout<<"客户端开启远端链接成功"<<endl;

    // 发送AddConn包告诉server端是哪个服务器的链接
    auto addConnReq = make_shared<Msg>(Msg());
    addConnReq->set_type(MSGTYPE_ADD_CONN_REQ);
    addConnReq->mutable_addconnreq()->mutable_conn_id()->append(connID);
    addConnReq->mutable_addconnreq()->mutable_client_id()->append(clientID);
    RPC rpc;
    unique_ptr<Msg> addConnRsp;
    // todo: 可以改用穿指针
    tie(addConnRsp, retCode) = rpc.Call(addConnReq, serverConn);
    if (retCode != 0) {
        cout<<"调用rpc新增链接失败"<<endl;
        return {retCode, "add conn client err"};
    }
    cout<<"客户端知会服务端connID:"<<connID<<endl;

    // 开启转发 读localConn写serConn 读serConn写localConn
    Forward f;
    f.AsyncForward(localConn, serverConn);

    // 回包成功
    return {0, "succ"};
}

