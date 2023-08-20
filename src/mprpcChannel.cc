#include "mprpcChannel.h"
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include "mprpcApplication.h"
#include "zookeeperUtil.h"

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
        controller->SetFailed("serialize request error!");
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
        controller->SetFailed("rpc header serialize error!");
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
        char errorTxt[512] = {0};
        sprintf(errorTxt, "create socket error! errno: %d", errno);
        controller->SetFailed(errorTxt);
        return;
    }


    //从zookeeper上读取host信息
    ZkClient zkClient;
    zkClient.Start();
    std::string methodPath = "/" + serviceName + "/" + methodName;
    std::string hostData = zkClient.GetData(methodPath.c_str());
    if(hostData == ""){
        controller->SetFailed(methodPath + " is not exist!");
        return;
    }
    int idx = hostData.find(":");
    if(idx == -1){
        controller->SetFailed(methodPath + " address is invalid!");
        return;
    }
    std::string ip = hostData.substr(0, idx);
    uint16_t port = atoi(hostData.substr(idx + 1, hostData.size() - idx).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    //链接rpc服务节点
    if(-1 == connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr))){
        char errorTxt[512] = {0};
        sprintf(errorTxt, "connect error! errno: %d", errno);
        controller->SetFailed(errorTxt);
        close(clientfd);
        return; 
    }

    //发送rpc请求
    if(-1 == send(clientfd, sendRpcStr.c_str(), sendRpcStr.size(), 0)){
        char errorTxt[512] = {0};
        sprintf(errorTxt, "send message error! errno: %d", errno);
        controller->SetFailed(errorTxt);
        close(clientfd);
        return;
    }

    //接收response
    char recvBuf[1024] = {0};
    int recvSize = 0;
    if(-1 == (recvSize = recv(clientfd, recvBuf, 1024, 0))){
        char errorTxt[512] = {0};
        sprintf(errorTxt, "recv message error! errno: %d", errno);
        controller->SetFailed(errorTxt);
        close(clientfd);
        return;
    }

    // std::string responseStr(recvBuf, 0, recvSize);
    if(!response->ParseFromArray(recvBuf, recvSize)){
        char errorTxt[512] = {0};
        sprintf(errorTxt, "parse response error! response string:%s", recvBuf);
        controller->SetFailed(errorTxt);
        close(clientfd);
        return;
    }

    close(clientfd);
}