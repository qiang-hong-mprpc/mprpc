#include <iostream>

#include "RpcConfig.h"

void RpcConfig::trim(std::string &str)
{
    int idx = str.find_first_not_of(' ');
    if (idx != -1)
    {
        str = str.substr(idx, str.size() - idx);
    }
    idx = str.find_last_not_of(' ');
    if (idx != -1)
    {
        str = str.substr(0, idx + 1);
    }
}

void RpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr)
    {
        std::cout << config_file << " is not existed!" << std::endl;
    }
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        std::string src_buf(buf);

        if (src_buf[0] == '#' || src_buf.empty())
        {
            continue;
        }
        int idx = src_buf.find('=');
        if (idx == -1)
        {
            continue;
        }
        std::string key = src_buf.substr(0, idx);
        trim(key);
        int end_idx = src_buf.find('\n', idx);
        std::string value = src_buf.substr(idx + 1, end_idx - idx - 1);
        trim(value);
        config_map.insert({key, value});
    }
}

std::string RpcConfig::Load(const std::string &key)
{
    auto it = config_map.find(key);
    if (it != config_map.end())
    {
        return it->second;
    }
    return "";
}