#include "frp-cpp/src/rpc.hpp"
#include "frp-cpp/src/network.hpp"
#include "frp-cpp/src/frame.hpp"
#include "frp-cpp/src/codec.hpp"
#include <memory>

using namespace FRP;
using namespace std;

tuple<unique_ptr<frp::Msg>, int32_t> Call(frp::Msg msg, shared_ptr<NetWorkOper> oper) {
    // 序列化
    Encoder encoder;
    auto data = encoder.Encode(msg);

    // 发送
    if (oper->Write((void*)data.c_str(), data.size()) == -1) {
        cout<<"登陆请求发送失败"<<endl;
        return {nullptr, -1};
    }

    // 等待回包
    Framer framer;
    auto rspData = framer.GetFrame(oper);

    // 解析
    Decoder decoder;
    auto rsp = decoder.Decode(rspData);
    return {move(rsp), int32_t(0)};
}