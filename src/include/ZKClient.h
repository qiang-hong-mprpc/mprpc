#pragma once

#include <zookeeper/zookeeper.h>
#include <semaphore.h> //信号量
#include <string>

class ZKClient
{
public:
    ZKClient();
    ~ZKClient();
    void Start();
    // 创建节点 路径 数据 大小 永久性还是临时性
    void Create(const char *path, const char *data, int datalen, int state = 0);
    std::string GetZnodeData(const char *path);

private:
    // 客户端句柄
    zhandle_t *m_zhandle;
};