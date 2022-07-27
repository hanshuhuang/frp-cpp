#!/bin/bash
echo `pwd`
workDir=`pwd`
mkdir -p ${workDir}/../.debug/
g++ ../src/*.cpp ../src/pb/*.h ../src/pb/*.cc Endian_test.cpp -I ${workDir}/../../ -lpthread -lprotobuf  --std=c++14 -g -o ${workDir}/../.debug/endian