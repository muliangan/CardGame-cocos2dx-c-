#pragma once
#include "cocos2d.h"
#include "../Classes/configs/models/LevelConfig.h"
USING_NS_CC;

//记录一个关卡需要的卡牌ui的初始位置，花色等，ui会在CardView根据卡牌和花色初始化
struct CardResConfig {
    int id;
    int suitImage;     // 花色
    int bigRankImage;     // 点数
    cocos2d::Vec2 playfieldPos; // 桌面区初始位置
    cocos2d::Vec2 stackPos;     // 手牌堆初始位置
    float scale = 1.0f;        // 缩放比例
    static std::vector<CardResConfig> loadPlayfieldConfigs(const LevelConfig& level);
    static std::vector<CardResConfig> loadStackConfigs(const LevelConfig& level);
};