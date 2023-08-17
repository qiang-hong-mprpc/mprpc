#include <iostream>
#include <string>
#include <google/protobuf/descriptor.h>
#include <sys/socket.h>
#include <error.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "MprpcChannel.h"
#include "client_request.pb.h"
#include "MprpcApplication.h"
#include "MprpcController.h"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *descriptor = method->service();
    std::string service_name = descriptor->name();
    std::string method_name = method->name();

    int args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serializa request error!");
        return;
    }

    mprpc::ClientRequest client_request;
    client_request.set_service_name(service_name);
    client_request.set_method_name(method_name);
    client_request.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (client_request.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("Serialize rpc header error!");
        return;
    }

    std::cout << "======================================" << std::endl;
    std::cout << "header size: " << header_size << std::endl;
    std::cout << "rpc header str: " << rpc_header_str << std::endl;
    std::cout << "service name: " << service_name << std::endl;
    std::cout << "method name: " << method_name << std::endl;
    std::cout << "args size: " << args_size << std::endl;
    std::cout << "args str: " << args_str << std::endl;
    std::cout << "======================================" << std::endl;

    // format: header_size + rpc_header_str + service_name + method_nam + args_size+ + args(struct client_request)
    std::string send_str;
    send_str.insert(0, std::string((char *)&header_size, 4));
    send_str += rpc_header_str + args_str;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        char err_txt[128];
        sprintf(err_txt, "create socket error! content: %d", errno);
        controller->SetFailed(err_txt);
        return;
    }

    std::string rpc_service_ip = MprpcApplication::GetInstance().GetRpcConfig().Load("rpc_service_ip");
    uint16_t rpc_service_port = std::atoi(MprpcApplication::GetInstance().GetRpcConfig().Load("rpc_service_port").c_str());

    struct sockaddr_in service_addr;
    service_addr.sin_family = AF_INET;
    service_addr.sin_port = htons(rpc_service_port);

    service_addr.sin_addr.s_addr = inet_addr(rpc_service_ip.c_str());

    if (-1 == connect(fd, (struct sockaddr *)&service_addr, sizeof(service_addr)))
    {
        char err_txt[128];
        sprintf(err_txt, "connect error! content: %d", errno);
        controller->SetFailed(err_txt);
        close(fd);
        return;
    }

    if (-1 == send(fd, send_str.c_str(), send_str.size(), 0))
    {
        char err_txt[128];
        sprintf(err_txt, "send error! content: %d", errno);
        controller->SetFailed(err_txt);
        close(fd);
        return;
    }

    char buf[1024] = {};
    int size = 0;
    if (-1 == (size = recv(fd, buf, 1024, 0)))
    {
        char err_txt[128];
        sprintf(err_txt, "recv error! content: %d", errno);
        controller->SetFailed(err_txt);
        close(fd);
        return;
    }

    std::string response_str(buf, 0, size);
    if (!response->ParseFromString(response_str))
    {
        char err_txt[128];
        sprintf(err_txt, "parse error! content: %d", errno);
        controller->SetFailed(err_txt);
        close(fd);
        return;
    }
    close(fd);
}