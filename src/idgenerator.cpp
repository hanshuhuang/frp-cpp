#include "frp-cpp/src/idgenerator.hpp"
#include <atomic>

using namespace FRP;
using namespace std;

static atomic<int> id(0);

string IDGenerator::GetConnID() {
    int connID = ++id;
    return "conn_" + to_string(connID);
}

string IDGenerator::GetClientID() {
    int cliID = ++id;
    return "cli_" + to_string(cliID);
}