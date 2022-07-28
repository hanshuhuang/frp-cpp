#include "TCPClient.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include "error.hpp"
#include <iostream>
#include <memory>
#include "TCPConnector.hpp"

using namespace FRP;
using namespace std;

int TCPClient::Connect() {
    struct sockaddr_in remoteAddr;
    bzero(&remoteAddr, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    remoteAddr.sin_port = htons(port);

    int clientFD = socket(PF_INET, SOCK_STREAM, 0);
    if (clientFD == -1) {
        cout<<"创建socket失败"<<strerror(clientFD)<<endl;
        return ERR_SOCK_ERR;
    }
    
    auto ret = connect(clientFD, (struct sockaddr*)&remoteAddr, sizeof(struct sockaddr));
    if (ret < 0) {
        cout<<"发起链接失败"<<strerror(ret)<<endl;
        return ERR_CONNECT_ERR;
    }

    c = make_unique<TCPConnector>(TCPConnector(clientFD));
    return 0;
}

TCPClient::TCPClient(const std::string& ip, uint16_t port) {
    this->ip = ip;
    this->port = port;
}

ssize_t TCPClient::ReadN(void* buf, size_t count) {
    return c->ReadN(buf, count);
}

ssize_t TCPClient::Write(void* buf, size_t count) {
    return c->Write(buf, count);
}

ssize_t TCPClient::Read(void *buf, size_t count) {
    return c->Read(buf, count);
}

void TCPClient::Close() {
    c->Close();
}

TCPClient::~TCPClient() {

}

TCPClient::TCPClient(FRP::TCPClient&& c) {
    this->ip = c.ip;
    this->port = c.port;
    this->c = move(c.c);
}