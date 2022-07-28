#include "frp-cpp/src/TCPConnector.hpp"
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace FRP;
using namespace std;

TCPConnector::TCPConnector(int sockfd) {
    this->sockfd = sockfd;
    // todo: 根据该sockfd填充必要信息
    // todo: 想改成非阻塞读写, 即epoll 
}

ssize_t TCPConnector::Read(void *buf, size_t count) {
    ssize_t nleft;
    ssize_t nread;
    nleft=(ssize_t)count;
    char* ptr;
    ptr=(char*)buf;
    nread=0;
    while(nleft>0){
        if((nread=read(sockfd,ptr,nleft))<0){
            if(errno==EINTR){
                nread=0;
            } else{
                cout<<"read err"<<strerror(errno)<<endl;
                return -1;
            }
        } else if(nread==0){  //read返回0代表读到EOF
            break;
        } else{
            // nleft-=nread;
            // ptr+=nread;
            // 不继续读了
            break;
        }
    }
    return nread;
}

ssize_t TCPConnector::ReadN(void *buff, size_t count) {
    ssize_t nleft;
    ssize_t nread;
    nleft=(ssize_t)count;
    char* ptr;
    ptr=(char*)buff;
    nread=0;
    while(nleft>0){
        if((nread=read(sockfd,ptr,nleft))<0){
            if(errno==EINTR){
                nread=0;
            } else{
                cout<<"read err"<<strerror(errno)<<endl;
                return -1;
            }
        } else if(nread==0){  //read返回0代表读到EOF
            break;
        } else{
            nleft-=nread;
            ptr+=nread;
        }
    }
    return (count-nread);
}

ssize_t TCPConnector::Write(void *buff, size_t count) {
    ssize_t nleft;
    ssize_t nwrite;
    char* ptr;

    nleft=(ssize_t)count;
    nwrite=0;
    ptr=(char*)buff;

    while(nleft>0){
        if((nwrite=write(sockfd,ptr,nleft))<=0){  //write返回0代表出错
            if(nwrite<0 && errno==EINTR){
                nwrite=0;
            }
            else{
                return -1;
            }
        }
        else{
            nleft-=nwrite;
            ptr+=nwrite;
        }
    }
    return (count);
}

void TCPConnector::Close() {
    close(this->sockfd);
}

TCPConnector::~TCPConnector() {

}