#pragma once
#include <string>
#include "../Classes/configs/models/LevelConfig.h"

// 关卡配置加载器
class LevelConfigLoader {
public:
    // 通过关卡编号加载关卡配置
    static LevelConfig loadLevelConfig(int levelNumber);
    
    // 通过关卡ID加载关卡配置
    static LevelConfig loadLevelConfig(const std::string& levelId);
    
    // 从 JSON 文件路径加载关卡配置
    static LevelConfig loadFromJson(const std::string& filePath);
};
