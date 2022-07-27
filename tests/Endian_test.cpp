#include "frp-cpp/src/frame.hpp"
#include <iostream>

using namespace std;
using namespace FRP;

int main() {
    bool isBigEnd = Framer::checkBigEnd();
    cout<<"IsBigEnd:"<<isBigEnd<<endl;
    int data = 1;
    auto bigEndData = Framer::WriteUint32(data);
    cout<<"Original:"<<data<<" "<<
        "ReadUint32:"<<Framer::ReadUint32((char*)bigEndData.c_str())<<endl;
}