#include "TCPServer.hpp"
#include "frp-cpp/error/error.hpp"

using namespace FRP;
using namespace std;

TCPServer::TCPServer(const string& ip, const uint16_t& port) {
    this->ip = ip;
    this->port = port;
    listenFD = 0;
    bzero(&servaddr, sizeof(servaddr));
}

int TCPServer::listen() {
    listenFD = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFD == -1) {
        return ERR_LISTEN_FAIL;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    if (bind(listenFD, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        return ERR_BIND_FAIL;
    }
}