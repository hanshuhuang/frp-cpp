#include "frp-cpp/src/rpc.hpp"
#include "frp-cpp/src/network.hpp"
#include "frp-cpp/src/frame.hpp"
#include "frp-cpp/src/codec.hpp"
#include <memory>

using namespace FRP;
using namespace std;
using namespace frp;

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