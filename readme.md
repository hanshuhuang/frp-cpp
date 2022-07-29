## frp-cpp
重拾cpp，希望借由frp这个项目来熟悉cpp的网络编程，加深了解。

## 快速开始
frp_client和frp_server都有build.sh脚本，编译失败可以重新生成protobuf文件
>进src/pb/
```
./regenerate.sh
```

### 服务器
部署在公网机器, 请输入IP、Port
```
./server 127.0.0.1 3000
```

### 客户端
部署在内网机器
请输入公网服务器IP、公务服务器端口、本机端口、远端映射端口
```
./client ${你的公网IP} 3000 22 3001
```
目前实现了tcp转发，因此按照上面部署完后，可以使用ssh登陆内网机器
```
ssh -oPort=3001 ${内网用户名}@${公网IP地址}
```

## 贡献
详见[迭代计划](./docs/readme.md)