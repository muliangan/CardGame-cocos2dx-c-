#pragma once
#include <vector>
#include "cocos2d.h"
//设置每一张牌的配置文件和每一个关卡的配置文件
// 关卡中每一张卡牌的配置
struct CardConfig {
    int face;           // 牌点数 Rank
    int suit;           // 花色 Suit
    cocos2d::Vec2 pos;  // 初始位置
};

// 关卡配置（一个关卡对应一份）
struct LevelConfig {
    std::vector<CardConfig> playfieldCards;  // 桌面牌
    std::vector<CardConfig> stackCards;      // 手牌堆初始卡牌
};