#!/bin/bash
echo `pwd`
workDir=`pwd`
mkdir -p ${workDir}/../.debug/
g++ ../src/* TCPClient_test.cpp -I ${workDir}/../../ -lpthread -g -o ${workDir}/../.debug/tcpclient