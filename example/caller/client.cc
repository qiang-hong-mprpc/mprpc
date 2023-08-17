#include <iostream>
#include <string>

#include "MprpcApplication.h"
#include "user.pb.h"
#include "MprpcChannel.h"
#include "MprpcController.h"

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    example::UserServicesRpc_Stub stub(new MprpcChannel());
    MprpcController mprpc_controller;

    // get friend list
    example::GetFriendListRequest get_friend_list_request;
    get_friend_list_request.set_userid(1234);

    example::GetFriendListSerivceRpc_Stub friend_service_stub(new MprpcChannel());
    example::getFriendListResponse get_friend_list_response;
    friend_service_stub.getFriendList(&mprpc_controller, &get_friend_list_request, &get_friend_list_response, nullptr);
    if (mprpc_controller.Failed())
    {
        std::cout << "controller failed, error_text: " << mprpc_controller.ErrorText() << std::endl;
    }
    else
    {
        if (get_friend_list_response.rc().errcode() == 0)
        {
            std::cout << "rpc get friend list success: " << std::endl;
            for (int i = 0; i < get_friend_list_response.friends_size(); i++)
            {
                std::cout << "index: " << (i + 1) << " name: " << get_friend_list_response.friends(i) << std::endl;
            }
        }
        else
        {
            std::cout << "rpc get friend list response error: " << get_friend_list_response.rc().errmsg() << std::endl;
        }
    }

    // Register
    example::RegisterRequest register_request;
    register_request.set_id(1234);
    register_request.set_name("qiang.lv");
    register_request.set_pwd("1234");

    example::RegisterResponse register_response;

    stub.Register(&mprpc_controller, &register_request, &register_response, nullptr);

    if (mprpc_controller.Failed())
    {
        std::cout << "controller failed, error_text: " << mprpc_controller.ErrorText() << std::endl;
    }
    else
    {
        if (register_response.rc().errcode() == 0)
        {
            std::cout << "rpc register response success: " << register_response.success() << std::endl;
        }
        else
        {
            std::cout << "rpc register response error: " << register_response.rc().errmsg() << std::endl;
        }
    }

    // Login
    example::LoginRequest login_request;
    login_request.set_name("qiang.lv");
    login_request.set_pwd("1234");

    example::LoginResponse login_response;

    stub.Login(&mprpc_controller, &login_request, &login_response, nullptr);

    if (mprpc_controller.Failed())
    {
        std::cout << "controller failed, error_text: " << mprpc_controller.ErrorText() << std::endl;
    }
    else
    {
        if (login_response.rc().errcode() == 0)
        {
            std::cout << "rpc login response success: " << login_response.success() << std::endl;
        }
        else
        {
            std::cout << "rpc login response error: " << login_response.rc().errmsg() << std::endl;
        }
    }
    return 0;
}