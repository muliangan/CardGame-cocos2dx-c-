#include"CardResConfig.h"
#include "cocos2d.h"
#include <float.h>

std::vector<CardResConfig> CardResConfig::loadPlayfieldConfigs(const LevelConfig& level) {
    std::vector<CardResConfig> configs;
    int index = 0;
    
    if (level.playfieldCards.empty()) {
        return configs;
    }
    
    // 获取屏幕尺寸和原点，用于世界坐标计算
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    
    // PlayField 区域的高度（假设在屏幕上方）
    float playfieldHeight = 1500.0f;
    float stackHeight = visibleSize.height - playfieldHeight;
    
    // JSON中的位置是左下角，y坐标加580，将在GameView中转换为世界坐标中心点
    // 世界坐标系：窗口左下角为原点(0,0)
    for (const auto& c : level.playfieldCards) {
        CardResConfig cfg;
        cfg.id = index++;
        cfg.suitImage = c.suit;
        cfg.bigRankImage = c.face;
        
        // JSON位置是左下角，y坐标加580，将在GameView中根据CardView实际大小转换为世界坐标中心点
        cfg.playfieldPos = cocos2d::Vec2(c.pos.x, c.pos.y+580.0f);
        cfg.stackPos = cocos2d::Vec2(0, 0);
        configs.push_back(cfg);
    }
    return configs;
}
std::vector<CardResConfig> CardResConfig::loadStackConfigs(const LevelConfig& level) {
    std::vector<CardResConfig> configs;
    int index = 1000;  // Stack 卡牌ID从1000开始（避免与 PlayField 冲突）
    
    // 获取屏幕尺寸和原点，用于世界坐标计算
    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    auto origin = cocos2d::Director::getInstance()->getVisibleOrigin();
    
    // Stack 区域的高度（假设在屏幕底部）
    float playfieldHeight = 1500.0f;
    float stackHeight = visibleSize.height - playfieldHeight;
    
    // 屏幕中心X（中间线）- 世界坐标
    float screenCenterX = origin.x + visibleSize.width / 2.0f;
    
    // Stack区域垂直中心 - 世界坐标（相对于窗口左下角）
    float baseY = origin.y + stackHeight / 2.0f;
    
    // 卡牌摆放参数
    float topCardOffset = 200.0f;  // 最后一张（顶部牌）距离中间线的距离
    float cardSpacing = 100.0f;    // 左边每张卡牌错开的宽度
    
    size_t totalCards = level.stackCards.size();

    for (size_t i = 0; i < totalCards; ++i) {
        const auto& c = level.stackCards[i];
        CardResConfig cfg;
        cfg.id = index++;  // Stack 卡牌ID从1000开始递增
        cfg.suitImage = c.suit;
        cfg.bigRankImage = c.face;
        cfg.playfieldPos = cocos2d::Vec2(0, 0);

        // 第一张到最后一张从左到右摆放
        // 最后一张（索引为 totalCards-1）放在中间线右边，距离200
        // 其他卡牌从中间线左边开始，每张向左错开100
        // 所有位置都是世界坐标（相对于窗口左下角）
        if (i == totalCards - 1) {
            // 最后一张（顶部牌）放在中间线右边
            cfg.stackPos = cocos2d::Vec2(screenCenterX + topCardOffset, baseY);
        }
        else {
            // 其他卡牌从中间线左边开始，每张向左错开100
            // 第一张在中间线左边，第二张再向左100，以此类推
            float posX = screenCenterX - (totalCards - 1 - i) * cardSpacing;
            cfg.stackPos = cocos2d::Vec2(posX, baseY);
        }

        configs.push_back(cfg);
    }
    return configs;
}