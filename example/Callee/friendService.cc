#include <iostream>
#include <string>
#include "../friend.pb.h"
#include "mprpcApplication.h"
#include "rpcProvider.h"

class FriendService : public fixbug::FriendServiceRpc{
public:
    std::vector<std::string> GetFriendList(uint32_t userId){
        std::cout<< " doing local service: get friend list" <<std::endl;

        std::vector<std::string> friends;
        friends.push_back("li si");
        friends.push_back("wang wu");
        return friends;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListRequest* request,
                       ::fixbug::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done){
        uint32_t id = request->userid();

        std::vector<std::string> friends = GetFriendList(id);

        fixbug::ResultCode *resultCode = response->mutable_result();
        resultCode->set_errcode(200);
        resultCode->set_errmsg("");
        for(std::string &name : friends){
            std::string *p = response->add_friends();
            *p = name;
        }

        //执行回调
        done->Run();
    }
};

int main(int argc, char **argv){
    //调用框架的初始化
    MprpcApplication::Init(argc, argv);

    //provider是一个网络服务对象，把Userservice对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    //启动一个rpc服务节点，Run以后，进程进入阻塞，等待远程请求
    provider.Run();

    return 0;
}