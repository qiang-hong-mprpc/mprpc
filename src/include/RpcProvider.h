#pragma once

#include <memory>
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
};