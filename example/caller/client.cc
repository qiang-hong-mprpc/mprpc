#include <iostream>
#include <string>

#include "MprpcApplication.h"
#include "user.pb.h"
#include "MprpcChannel.h"

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    example::UserServicesRpc_Stub stub(new MprpcChannel());

    // Register
    example::RegisterRequest register_request;
    register_request.set_id(1234);
    register_request.set_name("qiang.lv");
    register_request.set_pwd("1234");

    example::RegisterResponse register_response;

    stub.Register(nullptr, &register_request, &register_response, nullptr);

    if (register_response.rc().errcode() == 0)
    {
        std::cout << "rpc register response success: " << register_response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc register response error: " << register_response.rc().errmsg() << std::endl;
    }


    // Login 
    example::LoginRequest login_request;
    login_request.set_name("qiang.lv");
    login_request.set_pwd("1234");

    example::LoginResponse login_response;

    stub.Login(nullptr, &login_request, &login_response, nullptr);

    if (login_response.rc().errcode() == 0)
    {
        std::cout << "rpc login response success: " << login_response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << login_response.rc().errmsg() << std::endl;
    }
    return 0;
}