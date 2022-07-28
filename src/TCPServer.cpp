#include "TCPServer.hpp"
#include "frp-cpp/src/error.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <memory>
#include <thread>

using namespace FRP;
using namespace std;

TCPServer::TCPServer(const string& ip, const uint16_t& port) {
    this->ip = ip;
    this->port = port;
    listenFD = 0;
    bzero(&servaddr, sizeof(servaddr));
}

int TCPServer::Listen() {
    listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFD == -1) {
        cout<<"listen err:"<<strerror(errno);
        return ERR_SOCK_ERR;
    }

    int opt = 1;
    setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(listenFD, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        cout<<"bind err:"<<strerror(errno);
        return ERR_BIND_ERR;
    }

    if (listen(listenFD, 1024) == -1) {
        cout<<"listen err:"<<strerror(errno);
        return ERR_LISTEN_ERR;
    }

    return SUCC;
}

/*
    监听accept的数据，每个新链接都用一个线程来处理
    把链接封装成TCPConnector, 使用uniq_ptr来管理
    防止线程使用同一个数据
*/
void TCPServer::Loop(CallBack cb) {
    if (Listen() != SUCC) {
        cout<<"listen err"<<endl;
        return;
    }

    cout<<"start listen"<<endl;
    while (true) {
        struct sockaddr_in addr;
        int connFD = accept(listenFD, nullptr, nullptr);
        if (connFD == -1) {
            cout<<"accept err:"<<strerror(errno)<<endl;
            return;
        }
        
        auto handler = std::thread([&cb, &connFD](){
            cb(make_unique<TCPConnector>(connFD));
        });
        handler.detach();
    }
    cout<<"end listen"<<endl;
    // todo: 跳出循环是否需要close？
}