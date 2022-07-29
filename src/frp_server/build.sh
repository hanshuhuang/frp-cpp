#!/bin/bash
echo `pwd`
workDir=`pwd`
mkdir -p ${workDir}/../../.debug/
g++ ../*.cpp ../pb/*.cc ../frp_server/*.cpp -I ${workDir}/../../../  -lpthread `pkg-config --libs protobuf`  --std=c++14 -g -o ${workDir}/../../.debug/server