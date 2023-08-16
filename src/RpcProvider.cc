#include <string>
#include <functional>
#include <iostream>
#include <google/protobuf/descriptor.h>

#include "RpcProvider.h"
#include "MprpcApplication.h"

void RpcProvider::NotifyService(::google::protobuf::Service *service)
{
    ServiceInfo service_info;
    const google::protobuf::ServiceDescriptor *descriptor = service->GetDescriptor();
    const std::string service_name = descriptor->name();
    int service_message_count = descriptor->method_count();
    std::cout << "service name: " << service_name << std::endl;
    for (int i = 0; i < service_message_count; i++)
    {
        const google::protobuf::MethodDescriptor *method = descriptor->method(i);
        std::string method_name = method->name();
        service_info.provider_method_map.insert({method_name, method});
        std::cout << "method name: " << method_name << std::endl;
    }
    service_info.service = service;
    serivce_map.insert({service_name, service_info});
}

void RpcProvider::Run()
{
    std::string rpc_service_ip = MprpcApplication::GetInstance().GetRpcConfig().Load("rpc_service_ip");
    uint16_t rpc_service_port = std::atoi(MprpcApplication::GetInstance().GetRpcConfig().Load("rpc_service_port").c_str());

    muduo::net::InetAddress address(rpc_service_ip, rpc_service_port);

    // 创建 tcp service
    muduo::net::TcpServer tcp_service(&mprpc_event_loop, address, "rpc_provider");
    // 绑定连接、消息读写回掉
    tcp_service.setMessageCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    tcp_service.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // 线程数量
    tcp_service.setThreadNum(4);

    std::cout << "rpc provider start service at ip: " << rpc_service_ip << " port: " << rpc_service_port << std::endl;
    tcp_service.start();
    mprpc_event_loop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp)
{
}
