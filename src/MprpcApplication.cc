#include <iostream>
#include <unistd.h>
#include <string>

#include "MprpcApplication.h"

RpcConfig MprpcApplication::rpc_config;

void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    // get command
    int ch;
    std::string config;
    while ((ch = getopt(argc, argv, "i:")) != -1)
    {
        switch (ch)
        {
        case 'i':
            config = optarg;
            break;
        case '?':
            std::cout << "invalid args!" << std::endl;
            ShowArgsHelp();
            break;
        case ':':
            std::cout << "invalid args!" << std::endl;
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // read and load config file
    //  ip port zookeeper_ip zookeeper_port
    rpc_config.LoadConfigFile(config.c_str());
    std::cout << "rpc service ip: " << rpc_config.Load("rpc_service_ip") << std::endl;
    std::cout << "rpc service port: " << rpc_config.Load("rpc_service_port") << std::endl;
    std::cout << "zookeeper ip: " << rpc_config.Load("zookeeper_ip") << std::endl;
    std::cout << "zookeeper port: " << rpc_config.Load("zookeeper_port") << std::endl;
}

MprpcApplication &MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

RpcConfig &MprpcApplication::GetRpcConfig()
{
    return rpc_config;
}