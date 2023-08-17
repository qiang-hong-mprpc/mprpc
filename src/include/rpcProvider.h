#pragma once
#include "google/protobuf/service.h"
#include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <string>
#include <functional>
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <unordered_map>

// 框架提供的专门服务发布rpc服务的网络对象类
class RpcProvider{
public:
    //框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    //启动rpc服务节点，
    void Run();
private:
    //组合EventLoop
    muduo::net::EventLoop m_eventLoop;

    struct ServiceInfo{
        google::protobuf::Service *m_service; //保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap; //保存服务对象的方法
    };

    //保存所有注册成功的服务
    std::unordered_map<std::string, ServiceInfo> m_serviceInfoMap;

    //新的socket链接回调
    void OnConnection(const muduo::net::TcpConnectionPtr&);

    //以建立连接用户的读写事件回调
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
};