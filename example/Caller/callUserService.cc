#include <iostream>
#include "mprpcApplication.h"
#include "../user.pb.h"
#include "mprpcChannel.h"

int main(int argc, char **argv){

    //想使用mprpc框架，就要先调用框架初始化函数
    MprpcApplication::Init(argc, argv);

    //调用远程发布的rpc方法
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);

    //调用完成，读取结果
    if(200 == response.result().errcode() ){
        std::cout<< "rpc login success: " << response.success() << std::endl;
    } else {
        std::cout<< "rpc login error: " << response.result().errmsg() << std::endl;
    }

    return 0;
}