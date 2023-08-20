#include "zookeeperUtil.h"
#include "mprpcApplication.h"
#include <semaphore.h>
#include <iostream>
#include "logger.h"
 
//全局的watcher观察器
void GlobalWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcherCtx){
    if(type == ZOO_SESSION_EVENT){
        if(state == ZOO_CONNECTED_STATE){
            sem_t *sem = (sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

ZkClient::ZkClient() : m_zhandle(nullptr){}

ZkClient::~ZkClient(){
    if(m_zhandle != nullptr){
        zookeeper_close(m_zhandle);
    }
}

void ZkClient::Start(){
    std::string host = MprpcApplication::GetInstance().GetMprpcConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetMprpcConfig().Load("zookeeperport");
    std::string connstr = host + ":" + port;

    //ip:端口号 回调函数 超时时间
    m_zhandle = zookeeper_init(connstr.c_str(), GlobalWatcher, 30000, nullptr, nullptr, 0);

    if(nullptr == m_zhandle){
        std::cout<< "zookeeper init error!" << std::endl;
        LOG_ERROR("zookeeper init error");
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);
    std::cout<< "zookeeper initializing" << std::endl;

    sem_wait(&sem);
    std::cout<< "zookeeper init success!" << std::endl;
    LOG_INFO("zookeeper init success!");
}

void ZkClient::Create(const char *path, const char *data, int dataLen, int state){
    char pathBuffer[128];
    int bufferlen = sizeof(pathBuffer);
    int flag;
    flag = zoo_exists(m_zhandle, path, 0, nullptr);
    //先判断节点是否存在，ZNONODE表示不存在
    if(ZNONODE == flag){
        flag = zoo_create(m_zhandle, path, data, dataLen, &ZOO_OPEN_ACL_UNSAFE, state, pathBuffer, bufferlen);
        if(flag == ZOK){
            std::cout<<"create node success, path: " << path << std::endl;
            LOG_INFO("create node success, path: %s", path);
        } else {
            std::cout << "flag: " << flag << std::endl;
            LOG_ERROR("flag: %d", flag);
            std::cout << "znode create error!  path: " << path << std::endl;
            LOG_ERROR("znode create error!  path: %s", path);
            exit(EXIT_FAILURE);
        }
    }
}

//根据指定的path获取zoonode节点值 
std::string ZkClient::GetData(const char *path){
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if(flag != ZOK){
        std::cout << "get znode error!   path: " << path << std::endl;
        LOG_ERROR("get znode error!   path: %s", path);
        return "";
    } else {
        return buffer;
    }
}