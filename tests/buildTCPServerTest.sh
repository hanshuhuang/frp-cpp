#!/bin/bash
echo `pwd`
workDir=`pwd`
mkdir -p ${workDir}/../.debug/
g++ ../src/* TCPServer_test.cpp -I ${workDir}/../../ -lpthread -g -o ${workDir}/../.debug/tcpserver