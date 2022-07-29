#include "frp-cpp/src/rpc.hpp"
#include "frp-cpp/src/network.hpp"
#include "frp-cpp/src/frame.hpp"
#include "frp-cpp/src/codec.hpp"
#include <memory>

using namespace FRP;
using namespace std;
using namespace frp;

std::tuple<std::unique_ptr<frp::Msg>, int32_t> RPC::Recv(std::shared_ptr<NetWorkOper> oper) {
    Framer f;
    auto reqData = f.GetFrame(oper);
    auto msg = make_unique<Msg>(Msg());
    if (!msg->ParseFromString(reqData)) {
        cout<<"接收数据失败"<<endl;
        return {nullptr, -1};
    }

    return {move(msg), 0};
}

int32_t RPC::Send(std::shared_ptr<frp::Msg> msg, std::shared_ptr<NetWorkOper> oper) {
    Framer f;
    string data;
    int32_t retcode = 0;
    tie(data, retcode) = f.WriteFrame(msg);
    if (retcode != 0) {
        cout<<"发包错误"<<endl;
        return -1;
    }

    if (oper->Write((void*)data.c_str(), data.size()) <= 0) {
        return -1;
    }

    return 0;
}

std::tuple<std::unique_ptr<frp::Msg>, int32_t> RPC::Call(shared_ptr<frp::Msg> msg, std::shared_ptr<NetWorkOper> oper) {
    // 序列化
    Framer framer;
    string reqData;
    int32_t ret;
    tie(reqData, ret) = framer.WriteFrame(msg);
    if (ret != 0) {
        cout<<"写帧失败"<<endl;
        return {nullptr, ret};
    }

    // 发送
    if (oper->Write((void*)reqData.c_str(), reqData.size()) == -1) {
        cout<<"登陆请求发送失败"<<endl;
        return {nullptr, -1};
    }

    // 等待回包
    auto rspData = framer.GetFrame(oper);

    // 解析
    Decoder decoder;
    auto rsp = decoder.Decode(rspData);
    if (rsp == nullptr) {
        cout<<"反序列化数据失败"<<endl;
        return {nullptr, -1};
    }
    return {move(rsp), int32_t(0)};
}