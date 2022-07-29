#include "frp-cpp/src/frp_server/LinkManager.hpp"

using namespace std;
using namespace FRP;

static std::mutex mu;
static std::map<std::string, std::shared_ptr<TCPConnector>> links;

void LinkManager::Add(const std::string& connID, std::shared_ptr<TCPConnector> c) {
    lock_guard<mutex> lg(mu);
    links[connID] = c;
}

std::shared_ptr<TCPConnector> LinkManager::Pop(const std::string& connID) {
    lock_guard<mutex> lg(mu);
    auto it = links.find(connID);
    if (it == links.end()) {
        return nullptr;
    } 

    auto res = it->second;
    links.erase(it);
    return res;
}