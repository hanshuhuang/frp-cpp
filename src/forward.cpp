#include "frp-cpp/src/forward.hpp"
#include <iostream>
#include <string.h>
#include <thread>

using namespace FRP;
using namespace std;

const int buffSize = 4096;

void Forward::Copy(std::shared_ptr<NetWorkOper> src, std::shared_ptr<NetWorkOper> dst) {
    char buff[buffSize];
    while (true) {
        auto rret = src->Read((void*)buff, buffSize);
        if (rret <= 0) {
            cout<<"读错误,结束转发"<<endl;
            return;
        }
        cout<<"读到的数据"<<rret<<endl;

        auto wret = dst->Write((void*)buff, rret);
        if (wret <= 0) {
            cout<<"写错误,结束转发"<<endl;
            return;
        }
    }
}

void Forward::AsyncForward(
    std::shared_ptr<NetWorkOper> nw1, std::shared_ptr<NetWorkOper> nw2) {
    // 注意lambda不能用&，引用传递会导致无法正确incr，在Async函数退出后造成野指针
    auto c1 = std::thread([=](){
        Copy(nw1, nw2);
    });
    c1.detach();

    auto c2 = std::thread([=](){
        Copy(nw2, nw1);
    });
    c2.detach();
}