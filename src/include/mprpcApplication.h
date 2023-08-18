#pragma once

#include "mprpcConfig.h"
#include "mprpcChannel.h"
#include "mprpcController.h"

//mprpc框架初始化
class MprpcApplication{
public:
    static void Init(int argc, char **argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig& GetMprpcConfig();
private:
    static MprpcConfig m_config;

    MprpcApplication(){};
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
};