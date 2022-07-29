#ifndef FRP_LINKMANAGER_H
#define FRP_LINKMANAGER_H

#include <map>
#include <mutex>
#include <memory>
#include "frp-cpp/src/TCPConnector.hpp"

namespace FRP
{

class LinkManager {
public:
    static void Add(const std::string& connID, std::shared_ptr<TCPConnector> c);

    // 如果不存在返回空
    static std::shared_ptr<TCPConnector> Pop(const std::string& connID);

private:
    static std::mutex mu;
    static map<std::string, std::shared_ptr<TCPConnector>> links;
}

} // namespace FRP


#endif