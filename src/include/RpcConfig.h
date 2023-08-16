#include <unordered_map>
#include <string>

class RpcConfig
{
public:
    void LoadConfigFile(const char *config_file);
    // 查询配置项信息
    std::string Load(const std::string &key);

private:
    std::unordered_map<std::string, std::string> config_map;

    void trim(std::string &str);

};