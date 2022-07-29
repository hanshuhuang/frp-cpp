#!/bin/bash
echo `pwd`
workDir=`pwd`
mkdir -p ${workDir}/../.debug/
g++ ../src/*.cpp ../src/pb/*.cc ../src/frp_client/*.cpp ../src/frp_server/*.cpp FRPServer_test.cpp -I ${workDir}/../../  -lpthread `pkg-config --libs protobuf`  --std=c++14 -g -o ${workDir}/../.debug/frpserver