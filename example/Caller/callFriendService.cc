#include <iostream>
#include "mprpcApplication.h"
#include "../friend.pb.h"
#include "mprpcChannel.h"

int main(int argc, char **argv){

    //想使用mprpc框架，就要先调用框架初始化函数
    MprpcApplication::Init(argc, argv);

    //调用远程发布的rpc方法
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendListRequest request;
    request.set_userid(1);
    fixbug::GetFriendListResponse response;
    stub.GetFriendList(nullptr, &request, &response, nullptr);

    //调用完成，读取结果
    if(200 == response.result().errcode() ){
        std::cout<< "rpc get friend list success!" << std::endl;
        int size = response.friends_size();
        for(int i=0;i<size;i++){
            std::cout<< "index " << i + 1 << ": " << response.friends(i) << std::endl;
        }
    } else {
        std::cout<< "rpc get friend list error: " << response.result().errmsg() << std::endl;
    }

    return 0;
}