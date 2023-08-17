#include "rpcProvider.h"
#include <string>
#include "mprpcApplication.h"
#include <functional>
#include <iostream>

void RpcProvider::NotifyService(google::protobuf::Service *service){

}

void RpcProvider::Run(){
    std::string ip = MprpcApplication::GetInstance().GetMprpcConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetMprpcConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    //创建tcpserver对象
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    //绑定链接回调和消息读写回调方法
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    //设置muduo的线程链数
    server.setThreadNum(4);

    std::cout<< "Rpc provider start at ip:" << ip << " port:" << port << std::endl;
    //启动网络服务
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn){

}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp){

}
