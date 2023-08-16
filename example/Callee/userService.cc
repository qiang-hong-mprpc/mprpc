#include <iostream>
#include <string>
#include "../user.pb.h"
#include "mprpcApplication.h"
#include "rpcProvider.h"

using namespace std;

class UserService :public fixbug::UserServiceRpc {
public:
    bool Login(string name, string pwd){
        cout<< " doing local service: login" <<endl;
        cout<< "name: " << name << "  pwd: " << pwd << endl;
        return true;
    }

    //重写基类UserServiceRpc的虚函数方法 下面的方法都是框架直接调用的
    /*
     * 1. caller ===>  Login(LoginRequest) => muduo => callee
     * 2. callee => Login(LoginRequest) ==> 交到重写的Login方法上
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done){
        //框架给业务上报请求参数
        string name = request->name();
        string pwd = request->pwd();

        //业务代码
        bool loginResult = Login(name, pwd);

        //封装响应内容
        fixbug::ResultCode *resultCode = response->mutable_result();
        resultCode->set_errcode(200);
        resultCode->set_errmsg("success");
        response->set_success(loginResult);

        //执行回调
        done->Run();
    }

};

int main(int argc, char **argv){
    //调用框架的初始化
    MprpcApplication::Init(argc, argv);

    //provider是一个网络服务对象，把Userservice对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    //启动一个rpc服务节点，Run以后，进程进入阻塞，等待远程请求
    provider.Run();

    return 0;
}