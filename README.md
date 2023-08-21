# mprpc

### Project Introduction
The project mainly builds an operational framework based on remote communication services, aiming to complete multiple clients' access to remote servers, including service discovery, service registration, remote service heartbeat mechanism, and remote service invocation

### project use



### project column name 
```
bin: store binary files

build: intermediate build file

lib: store lib files

src: project source code

example: client and serv

build.sh: build ptoject, generate project binary and lib files
```


### dependence library
- boost
- muduo_net muduo_base
- pthread
- protobuf
- zookeeper(zookeeper_mt) multithreaded version


### tech stack & dev env
- language: c++ 
- build tool: cmake
- build env: ubuntu20.04 + vscode



### source code address
`https://github.com/qiang-hong-mprpc/mprpc`


### reference address
```
install boost: https://blog.csdn.net/qq_41854911/article/details/119454212 

protobuf address: git@github.com:qiang-hong-mprpc/protobuf.git

muduo address: git@github.com:qiang-hong-mprpc/muduo.git

install muduo lib: https://blog.csdn.net/QIANGWEIYUAN/article/details/89023980

install protobuf lib: https://www.jianshu.com/p/1c2069254045

zookeeper desc: https://www.cnblogs.com/xinyonghu/p/11031729.html

install zookeeper: https://juejin.cn/post/6844903924172849166

zookeeper install problem: https://blog.csdn.net/juggte/article/details/126283337
```