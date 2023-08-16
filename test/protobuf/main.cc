#include "test.pb.h"
#include <iostream>
#include <string>

using namespace fixbug;
using namespace std;

int main(){
    // LoginReponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(200);
    // rc->set_errmessage("success");
    // rsp.set_success(true);

    LoginRequest req;
    req.set_name("zhangsan");
    req.set_pwd("123456");

    string sendStr;
    if(req.SerializeToString(&sendStr)){
        cout<< "request请求序列化：" <<sendStr.c_str() << endl;
    }

    //反序列化
    LoginRequest reqB;
    if(reqB.ParseFromString(sendStr)){
        cout<< "反序列化结果："<<endl;
        cout<< reqB.name() << endl;
        cout<< reqB.pwd() << endl;
    }

    //设置对象
    GetFriendListReponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(200);
    rc->set_errmessage("success");

    //设置list
    User *user1 = rsp.add_users();
    user1->set_age(20);
    user1->set_name("zhang san");
    user1->set_sex(User::man);

    User *user2 = rsp.add_users();
    user2->set_age(20);
    user2->set_name("li si");
    user2->set_sex(User::woman);

    cout<< "对象中list_size:" <<rsp.users_size()<<endl;
    cout<< "对象中的对象的成员变量：" <<rsp.result().errcode()<<endl;

    string sendStr1;
    if(rsp.SerializeToString(&sendStr1)){
        cout<< "序列化对象后的结果：" << sendStr1.c_str() << endl;
    }

    return 0;
}