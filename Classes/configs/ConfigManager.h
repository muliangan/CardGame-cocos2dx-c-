#pragma once
#include "../Classes/configs/models/LevelConfig.h"

class ConfigManager
{
public:
    // 读取 JSON 文件内容，返回字符串
    static std::string loadJsonFile(const std::string& filePath);

    // 检查文件是否存在
    static bool fileExists(const std::string& filePath);
};
