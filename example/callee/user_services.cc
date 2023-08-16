#include <iostream>
#include <string>

#include "../user.pb.h"
#include "MprpcApplication.h"
#include "RpcProvider.h"

class UserService : public example::UserServicesRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "name: " << name << std::endl;
        std::cout << "pwd: " << pwd << std::endl;
        return true;
    }

    // 框架直接调用 重写基类login函数
    void Login(::google::protobuf::RpcController *controller,
               const ::example::LoginRequest *request,
               ::example::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // request反序列化 解析 deal return
        std::string name = request->name();
        std::string pwd = request->pwd();

        // deal
        bool is_login = Login(name, pwd);

        // 返回response
        example::ResultCode *rc = response->mutable_rc();
        rc->set_errcode(0);
        rc->set_errmsg("success");
        response->set_success(is_login);

        // 回调函数 序列化和反序列化框架完成
        done->Run();
    }

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        return true;
    }

    void Register(::google::protobuf::RpcController *controller,
                  const ::example::RegisterRequest *request,
                  ::example::RegisterResponse *response,
                  ::google::protobuf::Closure *done)
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool is_login = Register(id, name, pwd);

        example::ResultCode *rc = response->mutable_rc();
        rc->set_errcode(0);
        rc->set_errmsg("success");
        response->set_success(is_login);

        // 回调函数 序列化和反序列化框架完成
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 服务初始化
    MprpcApplication::Init(argc, argv);

    // 提供服务对象到rpc节点
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动rpc节点服务 进入堵塞 直到用户调用
    provider.Run();

    return 0;
}