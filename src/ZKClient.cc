#include <iostream>

#include "ZKClient.h"
#include "MprpcApplication.h"

void ZookkeeperWatcher(zhandle_t *zh, int type, int state, const char *path, void *wahtcherCtx)
{
    if (type == ZOO_SESSION_EVENT)
    {
        if (state == ZOO_CONNECTED_STATE)
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZKClient::ZKClient() : m_zhandle(nullptr)
{
}

ZKClient::~ZKClient()
{
    if (m_zhandle != nullptr)
    {
        zookeeper_close(m_zhandle);
    }
}

void ZKClient::Start()
{
    std::string zookeeper_ip = MprpcApplication::GetInstance().GetRpcConfig().Load("zookeeper_ip");
    std::string zookeeper_port = MprpcApplication::GetInstance().GetRpcConfig().Load("zookeeper_port");

    std::string config_info = zookeeper_ip + ":" + zookeeper_port;
    m_zhandle = zookeeper_init(config_info.c_str(), ZookkeeperWatcher, 30000, nullptr, nullptr, 0);
    if (nullptr == m_zhandle)
    {
        std::cout << "zookeeper init error!" << std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    std::cout << "zookeeper init success!" << std::endl;
}

void ZKClient::Create(const char *path, const char *data, int datalen, int state)
{
    char buff[128];
    int buff_len = sizeof(buff);
    int node_exist = zoo_exists(m_zhandle, path, 0, nullptr);
    if (ZNONODE == node_exist)
    {
        int code = zoo_create(m_zhandle, path, data, datalen, &ZOO_OPEN_ACL_UNSAFE, state, buff, buff_len);
        if (ZOK == code)
        {
            std::cout << "znode create success... patth:" << path << std::endl;
        }
        else
        {
            std::cout << "znode create error,path:" << path << " code: " << code << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cout << "znode create error,path:" << path << " code: " << node_exist << std::endl;
    }
}

std::string ZKClient::GetZnodeData(const char *path)
{
    char buff[128];
    int buff_len = sizeof(buff);
    int code = zoo_get(m_zhandle, path, 0, buff, &buff_len, nullptr);
    if (code != ZOK)
    {
        std::cout << "get znode error... path:" << path << " code: " << code << std::endl;
        return "";
    }
    return buff;
}