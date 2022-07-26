#ifndef FRP_ERROR_H
#define FRP_ERROR_H

namespace FRP
{

enum err {
    SUCC = 0,
    ERR_SOCK_ERR = 1,
    ERR_BIND_ERR = 2,
    ERR_LISTEN_ERR
};


}

#endif