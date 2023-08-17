#include "../user.pb.h"
#include "MprpcApplication.h"
#include "RpcProvider.h"
#include "get_friend_list_services.h"
#include "user_services.h"


int main(int argc, char **argv)
{
    // 服务初始化
    MprpcApplication::Init(argc, argv);

    // 提供服务对象到rpc节点
    RpcProvider provider;
    provider.NotifyService(new GetFriendListSerivce());
    provider.NotifyService(new UserService());

    // 启动rpc节点服务 进入堵塞 直到用户调用
    provider.Run();

    return 0;
}