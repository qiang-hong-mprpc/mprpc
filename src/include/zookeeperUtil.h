#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>

//封装的zk客户端类
class ZkClient{
public:
    ZkClient();
    ~ZkClient();
    //zkclient启动链接zkserver
    void Start();
    //在zkserver上创建指定path的server节点
    void Create(const char *path, const char *data, int dataLen, int state=0);
    //根据路径获得node中的值
    std::string GetData(const char *path);
private:
    // zk的客户端句柄
    zhandle_t *m_zhandle;
};