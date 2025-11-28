#pragma once
#include "../models/GameModel.h"
#include "../configs/models/LevelConfig.h"
#include "../configs/models/CardResConfig.h"

// 游戏模型生成服务将静态配置（LevelConfig）转换为动态运行时数据（GameModel）
class GameModelFromLevelGenerator {
public:
    // 从关卡配置生成游戏模型
    // levelConfig - 关卡配置
    static GameModel* generateGameModel(const LevelConfig& levelConfig);
    
private:
    // 从 CardResConfig 创建 CardModel
    // config - 卡牌资源配置
    // isStack - 是否为堆叠区域的卡牌
    static CardModel* createCardModelFromConfig(const CardResConfig& config, bool isStack);
};

