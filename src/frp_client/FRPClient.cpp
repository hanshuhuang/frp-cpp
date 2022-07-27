#include "frp-cpp/src/pb/message.pb.h"
#include "frp-cpp/src/frp_client/FRPClient.hpp"
#include "frp-cpp/src/frame.hpp"
#include "frp-cpp/src/TCPClient.hpp"
#include "frp-cpp/src/codec.hpp"
#include "frp-cpp/src/rpc.hpp"
#include <memory>
#include <iostream>

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
        cout<<"登陆失败"<<endl;
        return -1;
    }

    // 请求登陆
    auto msg = Msg();
    msg.set_type(MSGTYPE_LOGIN_REQ);
    msg.mutable_loginreq()->mutable_localconf()->CopyFrom(localConf);

    RPC rpc;
    unique_ptr<Msg> rsp;
    int32_t retcode = 0;
    tie(rsp, retcode) = rpc.Call(msg);
    if (retcode != 0) {
        cout<<"发包失败"<<endl;
        return -1;
    }

    // 赋值
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

        switch (msg->type())
        {
        case MSGTYPE_ADD_CONN_REQ:
            AddConn(move(msg));
            break;
        default:
            cout<<"unknown command type"<<msg->type()<<endl;
        }
    }
}