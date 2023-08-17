#pragma once

#include <unordered_map>
#include <string>

// 框架读取配置文件类

class MprpcConfig{
public:
    //解析配置文件
    void LoadConfigFile(const char *config_file);
    //读取配置项信息
    std::string Load(std::string key);
private:
    std::unordered_map<std::string, std::string> m_configMap;
    //去掉字符串前后的空格
    void Trim(std::string &src_buf);
};