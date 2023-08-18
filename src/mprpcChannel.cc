#include "mprpcChannel.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "mprpcApplication.h"

// header_size + service_name method_name args_size + args
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response, 
                          google::protobuf::Closure* done){
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string  serviceName = sd->name();
    std::string methodName = method->name();

    //获取参数序列化字符串长度 argsSize
    int argsSize = 0;
    std::string argsStr;
    if(request->SerializeToString(&argsStr)){
        argsSize = argsStr.size();
    } else {
        std::cout<< "serialize request error!" << std::endl;
        return;
    }

    //定义请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(serviceName);
    rpcHeader.set_method_name(methodName);
    rpcHeader.set_args_size(argsSize);

    uint32_t headerSize = 0;
    std::string rpcHeaderStr;
    if(rpcHeader.SerializeToString(&rpcHeaderStr)){
        headerSize = rpcHeaderStr.size();
    } else {
        std::cout<< "rpc header serialize error!" << std::endl;
        return;
    }

    //组织发送rpc请求的字符串
    std::string sendRpcStr;
    sendRpcStr.insert(0, std::string((char*)&headerSize, 4));
    sendRpcStr= sendRpcStr + rpcHeaderStr;
    sendRpcStr = sendRpcStr + argsStr;

    std::cout<< "=================================================" <<std::endl;
    std::cout<< "header size:" << headerSize <<std::endl;
    std::cout<< "rpc header string:" << rpcHeaderStr <<std::endl;
    std::cout<< "service name:" << serviceName <<std::endl;
    std::cout<< "method name:" << methodName <<std::endl;
    std::cout<< "args string:" << argsStr <<std::endl;
    std::cout<< "=================================================" <<std::endl;

    //使用tcp编程，完成spr远程调用
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);

    if( -1 == clientfd){
        std::cout<< "create socket error! errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string ip = MprpcApplication::GetInstance().GetMprpcConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetMprpcConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    //链接rpc服务节点
    if(-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))){
        std::cout<< "connect error! errno: " << errno << std::endl;
        close(clientfd);
        exit(EXIT_FAILURE);
    }

    //发送rpc请求
    if(-1 == send(clientfd, sendRpcStr.c_str(), sendRpcStr.size(), 0)){
        std::cout<< "send message error! errno: " << errno << std::endl;
        close(clientfd);
        return;
    }

    //接收response
    char recvBuf[1024] = {0};
    int recvSize = 0;
    if(-1 == (recvSize = recv(clientfd, recvBuf, 1024, 0))){
        std::cout<< "recv message error! errno: " << errno << std::endl;
        close(clientfd);
        return;
    }

    // std::string responseStr(recvBuf, 0, recvSize);
    if(!response->ParseFromArray(recvBuf, recvSize)){
        std::cout<< "parse response error! response string: " << recvBuf <<std::endl;
        close(clientfd);
        return;
    }

    close(clientfd);
}