#include <string>
#include <functional>
#include <iostream>
#include <google/protobuf/descriptor.h>

#include "RpcProvider.h"
#include "MprpcApplication.h"
#include "client_request.pb.h"
#include "ZKClient.h"

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

    ZKClient ZkClient;
    ZkClient.Start();
    for (auto &service : serivce_map)
    {
        std::string service_path = "/" + service.first;
        ZkClient.Create(service_path.c_str(), nullptr, 0);
        for (auto &method : service.second.provider_method_map)
        {
            std::string method_path = service_path + "/" + method.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", rpc_service_ip.c_str(), rpc_service_port);
            ZkClient.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    std::cout << "rpc provider start service at ip: " << rpc_service_ip << " port: " << rpc_service_port << std::endl;
    tcp_service.start();
    mprpc_event_loop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // rpc client断开了
        conn->shutdown();
    }
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp timestamp)
{
    // 接收client的请求信息
    std::string recv_buf = buffer->retrieveAllAsString();
    std::cout << "receive message: " << recv_buf << std::endl;

    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);

    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::ClientRequest client_requst;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (client_requst.ParseFromString(rpc_header_str))
    {
        service_name = client_requst.service_name();
        method_name = client_requst.method_name();
        args_size = client_requst.args_size();
    }
    else
    {
        std::cout << "rpc header str: " << rpc_header_str << " parse error!" << std::endl;
        return;
    }
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    std::cout << "======================================" << std::endl;
    std::cout << "header size: " << header_size << std::endl;
    std::cout << "rpc header str: " << rpc_header_str << std::endl;
    std::cout << "service name: " << service_name << std::endl;
    std::cout << "method name: " << method_name << std::endl;
    std::cout << "args size: " << args_size << std::endl;
    std::cout << "args str: " << args_str << std::endl;
    std::cout << "======================================" << std::endl;

    auto it = serivce_map.find(service_name);
    if (it == serivce_map.end())
    {
        std::cout << service_name << " is not exist!" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.service;
    auto method_it = it->second.provider_method_map.find(method_name);
    if (method_it == it->second.provider_method_map.end())
    {
        std::cout << service_name << ":" << method_name << " is not exist!" << std::endl;
        return;
    }
    const google::protobuf::MethodDescriptor *method = method_it->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "request parse error, content:" << args_str << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // call method最后一个参数 自定义函数 要绑定一个Closure函数
    google::protobuf::Closure *done =
        google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(this, &RpcProvider::SendRpcResponse, conn, response);

    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if (response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else
    {
        std::cout << "serializa response error:" << response_str << std::endl;
    }
    conn->shutdown();
}
