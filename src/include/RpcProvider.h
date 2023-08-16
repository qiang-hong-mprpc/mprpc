#pragma once

#include <memory>
#include <unordered_map>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>

#include "google/protobuf/service.h"

class RpcProvider
{
public:
    void NotifyService(::google::protobuf::Service *service);
    void Run();

private:
    std::unique_ptr<muduo::net::TcpServer> mprpc_service_ptr;

    muduo::net::EventLoop mprpc_event_loop;
    void OnConnection(const muduo::net::TcpConnectionPtr &);
    void OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr &, google::protobuf::Message *);

    struct ServiceInfo
    {
        google::protobuf::Service *service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> provider_method_map;
    };
    std::unordered_map<std::string, ServiceInfo> serivce_map;
};