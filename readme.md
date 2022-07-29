## frp-cpp
重拾cpp，希望借由frp这个项目来熟悉cpp的网络编程，加深了解。

## 迭代计划
### v0.0.1(2022/07/23~2022/08/01)
#### 特性
实现SSH内网穿透

#### 实现思路
- 服务端
  - 对外开放管理端口(如6000)，并进行监听
    - 收到客户端的登陆请求，解析配置，监听对应的端口(比如3000)
    - 收到用户到(3000)的请求，则根据端口和客户端的映射，通过管理链接让客户端发起链接，链接携带监听的配置号，告知服务端该链接属于哪个端口
    - 根据客户端的链接进行tcp转发
- 客户端
  - 启动时登陆
  - 收到发起链接请求，则新开线程，连接服务端机器
  - 把随后的流量就行无脑转发到ssh端口

- 代码实现
  - 转发逻辑
    - 两个线程
      - 负责把读socketA写socketB
      - 负责读socketB写socketA
  - 登陆逻辑
    - 配置文件按照server纬度来和外网服务器通信，使用单独线程来和外网服务器通信，采用一应一答模式
      - 携带服务器对外开放的端口信息等传给外网服务器
      - 收到回包的clientID，更新clientID
  - 新增链接逻辑
    - 当外网服务器收到用户连接请求时候，会告诉内网服务器发起链接
    - 内网服务器携带clientID发起链接，开始转发(User->PublicSvr PublicSvr->User)
    - 外网服务器收到链接，并开始转发(PublicSvr->LocalHost LocalHost->PublicSvr)
  - 日志
    - 简单落盘即可

- 请求设计
  见pb目录，使用protobuf做传输协议即可 
  
### 待完成工作
- 多线程模型低效
- 日志打印低效
- 没有benchmark
- 存在低效拷贝
- 没有延迟测试
- 长链接维护
- 链接没有用RAII进行回收