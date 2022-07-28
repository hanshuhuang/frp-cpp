#!/bin/bash
echo `pwd`
workDir=`pwd`
mkdir -p ${workDir}/../.debug/
g++ ../src/*.cpp ../src/pb/*.h ../src/pb/*.cc ../src/frp_client/*.cpp Endian_test.cpp -I ${workDir}/../../ ${workDir}/../src/frp_client -lpthread -lprotobuf `pkg-config --libs protobuf`  --std=c++14 -g -o ${workDir}/../.debug/endian