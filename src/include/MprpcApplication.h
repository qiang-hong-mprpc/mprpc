#pragma once

#include "RpcConfig.h"

class MprpcApplication
{
public:
    static void Init(int argc, char **argv);
    static MprpcApplication &GetInstance();
    static RpcConfig &GetRpcConfig();

private:
    MprpcApplication() {}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;

    static RpcConfig rpc_config;
};