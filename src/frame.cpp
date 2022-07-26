#include "frp-cpp/src/frame.hpp"
#include <iostream>
#include <string>

using namespace FRP;
using namespace std;

std::string GetFrame(shared_ptr<NetWorkOper> net) {
    char lenBuf[4];
    if (net->ReadN(lenBuf, sizeof(lenBuf)) == -1) {
        cout<<"GetFrame 读取长度失败"<<endl;
        return "";
    }

    auto totalLen = ReadUint32(lenBuf);
    if (totalLen == 0) {
        cout<<"GetFrame 读取数据长度为0"<<endl;
        return "";
    }

    char* buf = new char[totalLen + 1];
    buf[totalLen] = 0;
    if (net->ReadN(buf, totalLen) == -1) {
        cout<<"GetFrame 读取payload失败"<<endl;
        return "";
    }

    // todo: 严重性能问题，待优化
    string payload(buf);
    delete[] buf;
    return payload;
}

/*
    假设数字为0x01020304
    低地址->高地址
    大端：01 02 03 04
    小端：04 03 02 01
*/
bool isBigEnd = []() {
    union un {
        int digital;
        char c;
    } u;
    u.digital = 1;

    // 因为c代表低地址，digital = 0x00 00 00 01
    // 如果是大端，c = 00
    // 如果是小端，c = 1
    if (u.c > 0) {
        return false;
    }
    return true;
}();

uint32_t ReadUint32(char* lenBuf) {
    uint32_t d;
    char* c = (char*)&d;
    // 因为网络序是大端，所以直接解析就行
    if (isBigEnd) {
        c[0] = lenBuf[0];
        c[1] = lenBuf[1];
        c[2] = lenBuf[2];
        c[3] = lenBuf[3];
    } // 小端，则需要翻转一下
    else {
        c[0] = lenBuf[3];
        c[1] = lenBuf[2];
        c[2] = lenBuf[1];
        c[3] = lenBuf[0];
    }
    return d;
}