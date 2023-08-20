#include "rpcProvider.h"
#include "mprpcApplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperUtil.h"

void RpcProvider::NotifyService(google::protobuf::Service *service){
    ServiceInfo serviceInfo;

    //获取用户对象的描述信息
    const google::protobuf::ServiceDescriptor *pServiceDesc = service->GetDescriptor();

    // 获取服务的信息
    std::string serviceName = pServiceDesc->name();
    int methodCount = pServiceDesc->method_count();
    LOG_INFO("service name: %s", serviceName.c_str());

    for(int i=0;i<methodCount;i++){
        const google::protobuf::MethodDescriptor *pmethodDesc = pServiceDesc->method(i);
        std::string methodName = pmethodDesc->name();
        serviceInfo.m_methodMap.insert({methodName, pmethodDesc});

        LOG_INFO("method name: %s", methodName.c_str());
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

    //把当前rpc节点上要发布的服务注册到zk上，让rpc client可以从zk上发现服务
    ZkClient zkClient;
    zkClient.Start();
    //server_name为永久性节点，method_name为临时性节点
    for(auto &sp : m_serviceInfoMap){
        std::string servicePath = "/" + sp.first;
        zkClient.Create(servicePath.c_str(), nullptr, 0);
        for(auto &mp : sp.second.m_methodMap){
            std::string methodPath = servicePath + "/" + mp.first;
            char methodPathData[128] = {0};
            sprintf(methodPathData, "%s:%d", ip.c_str(), port);
            //ZOO_EPHEMERAL表示临时性节点
            zkClient.Create(methodPath.c_str(), methodPathData, strlen(methodPathData), ZOO_EPHEMERAL);
        }
    }

    std::cout<< "Rpc provider start at ip:" << ip << " port:" << port << std::endl;
    //启动网络服务
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn){
    if(!conn->connected()){
        //链接断开
        conn->shutdown();
    }
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp){
    //网络上接收的远程rpc调用字符流
    std::string recv_buf = buffer->retrieveAllAsString();

    //从字符流中读取前4个字节内容
    uint32_t headerSize = 0;
    recv_buf.copy((char*)&headerSize, 4, 0);

    //根据headerSize读取原始字符流
    std::string rpc_header_str = recv_buf.substr(4, headerSize);
    mprpc::RpcHeader rpcHeader;
    std::string serviceName;
    std::string methodName;
    uint32_t argsSize;
    if(rpcHeader.ParseFromString(rpc_header_str)){
        //数据头反序列化成功
        serviceName = rpcHeader.service_name();
        methodName = rpcHeader.method_name();
        argsSize = rpcHeader.args_size();
    } else {
        //反序列化失败
        std::cout<<"rpc header string: " << rpc_header_str << " parse error!"<< std::endl;
        return;
    }

    std::string argsStr = recv_buf.substr(4 + headerSize, argsSize);

    std::cout<< "=================================================" <<std::endl;
    std::cout<< "header size:" << headerSize <<std::endl;
    std::cout<< "rpc header string:" << rpc_header_str <<std::endl;
    std::cout<< "service name:" << serviceName <<std::endl;
    std::cout<< "method name:" << methodName <<std::endl;
    std::cout<< "args string:" << argsStr <<std::endl;
    std::cout<< "=================================================" <<std::endl;

    //获取service和method对象
    auto it = m_serviceInfoMap.find(serviceName);
    if(it == m_serviceInfoMap.end()){
        std::cout<< serviceName <<" is not exist" << std::endl;
        return;
    }

    auto mit = it->second.m_methodMap.find(methodName);
    if(mit == it->second.m_methodMap.end()){
        std::cout<< serviceName << ":" << methodName <<" is not exist" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor *method = mit->second;

    //生成rpc调用方法request和response
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(argsStr)){
        std::cout<< "request parse error, content:" << argsStr << std::endl;

    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    //给方法调用绑定Closure回调
    google::protobuf::Closure *done = 
        google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, conn, response);

    //在框架上根据远端rpc请求，调用rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response){
    std::string responseStr;
    if(response->SerializeToString(&responseStr)){
        //序列化成功后，通过网络把response返回调用方
        conn->send(responseStr);
    } else {
        std::cout<< "serialize response error!" << std::endl;
    }
    conn->shutdown(); //模拟http短链接服务，由rpcprovider主动断开
}
