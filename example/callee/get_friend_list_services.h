#include <iostream>
#include <string>
#include <vector>

#include "../user.pb.h"
#include "MprpcApplication.h"
#include "RpcProvider.h"

class GetFriendListSerivce : public example::GetFriendListSerivceRpc
{
public:

    std::vector<std::string> GetFriendList(uint32_t user_id)
    {
        std::vector<std::string> ans;
        ans.push_back("zhang san");
        ans.push_back("li si");
        ans.push_back("wang wu");
        return ans;
    }

    void getFriendList(::google::protobuf::RpcController *controller,
                       const ::example::GetFriendListRequest *request,
                       ::example::getFriendListResponse *response,
                       ::google::protobuf::Closure *done)
    {
         // request反序列化 解析 deal return
        uint32_t user_id = request->userid();

        // deal
        std::vector<std::string> friend_list = GetFriendList(user_id);

        // 返回response
        example::ResultCode *rc = response->mutable_rc();
        rc->set_errcode(0);
        rc->set_errmsg("success");
        for(std::string &name : friend_list)
        {
            std::string *p = response->add_friends();
            *p = name;
        }

        // 回调函数 序列化和反序列化框架完成
        done->Run();
    }
};