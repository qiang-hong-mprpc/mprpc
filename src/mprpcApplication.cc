#include "mprpcApplication.h" 
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::m_config;

void ShwoArgsHelp(){
    std::cout << "format: command -i <configfile>" << std::endl;
}

void MprpcApplication::Init(int argc, char **argv){
    if(argc < 2){
        ShwoArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1){
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            std::cout<< "invalid args" <<std::endl;
            ShwoArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            std::cout<< "need <configfile>" <<std::endl;
            ShwoArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //加载配置文件 rpcserver_ip=   rpcserver_port=   zookepper_ip =      zookepper_port = 
    m_config.LoadConfigFile(config_file.c_str()); 

    std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    std::cout << "zookepperip:" << m_config.Load("zookepperip") << std::endl;
    std::cout << "zookepperport:" << m_config.Load("zookepperport") << std::endl;
}

MprpcApplication& MprpcApplication::GetInstance(){
    static MprpcApplication app;
    return app;
}
