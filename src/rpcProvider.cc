#include "rpcProvider.h"
#include "mprpcApplication.h"

void RpcProvider::NotifyService(google::protobuf::Service *service){
    ServiceInfo serviceInfo;

    //获取用户对象的描述信息
    const google::protobuf::ServiceDescriptor *pServiceDesc = service->GetDescriptor();

    // 获取服务的信息
    std::string serviceName = pServiceDesc->name();
    int methodCount = pServiceDesc->method_count();
    std::cout<< "service name: " << serviceName << std::endl;

    for(int i=0;i<methodCount;i++){
        const google::protobuf::MethodDescriptor *pmethodDesc = pServiceDesc->method(i);
        std::string methodName = pmethodDesc->name();
        serviceInfo.m_methodMap.insert({methodName, pmethodDesc});

        std::cout<< "method name: " << methodName << std::endl;
    }
    serviceInfo.m_service = service;
    m_serviceInfoMap.insert({serviceName, serviceInfo});
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
