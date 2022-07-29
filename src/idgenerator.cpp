#include "frp-cpp/src/idgenerator.hpp"

using namespace std;

string IDGenerator::GetConnID() {
    int connID = ++id;
    return "conn_" + to_string(connID);
}

string IDGenerator::GetClientID() {
    int cliID = ++id;
    return "cli_" + to_string(cliID);
}