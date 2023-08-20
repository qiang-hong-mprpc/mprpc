# mprpc

## 使用
- 在项目目录下创建路径bin、build、lib

- 在example下准备proto文件，运行 protoc xxx.proto --cpp_out=./ 生成对应的.cc和.h文件

- 运行autobuild.sh脚本，编译src和example下的内容

- example下的内容会编译成 provider 和 consumer

- 准备config文件，将test.conf文件移入bin目录下

- 运行 ./provider -i test.conf 启动服务端

- 运行 ./consumer -i test.conf 即可启动客户端向服务端访问对应服务


## 技术栈
1. muduo
2. protobuf
3. zookeeper

## 项目运行逻辑

### 服务端

- 服务端启动后，将proto文件中定义的service和包含的method全部注册进muduo server中

- 将muduo server和method以对应ip:port的形式，注册进zookeeper中

- 启动server，server会自动进入阻塞，等待被调用

- 每次调用被远端调用时，server会调用service的OnMessage方法

### 客户端

- 客户端启动时，初始化protobuf生成的stub类，这个stub类中有在服务端注册的所有服务信息

- 通过stub类，调用对应的方法

- 调用stub类的方法时，会根据方法的信息，从zookeeper中获取对应的ip:port

- 根据ip:port，建立tcp链接，并发送调用请求
