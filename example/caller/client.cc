#include <iostream>
#include <string>

#include "MprpcApplication.h"
#include "user.pb.h"
#include "MprpcChannel.h"

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    example::UserServicesRpc_Stub stub(new MprpcChannel());

    example::LoginRequest request;
    request.set_name("qiang.lv");
    request.set_pwd("1234");

    example::LoginResponse response;

    stub.Login(nullptr, &request, &response, nullptr);

    if (response.rc().errcode() == 0)
    {
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.rc().errmsg() << std::endl;
    }
    return 0;
}