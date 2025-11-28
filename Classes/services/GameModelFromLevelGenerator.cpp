#include "GameModelFromLevelGenerator.h"
#include "../models/CardModel.h"
#include "../configs/models/CardResConfig.h"

GameModel* GameModelFromLevelGenerator::generateGameModel(const LevelConfig& levelConfig)
{
    // 1. 从 LevelConfig 加载 CardResConfig
    std::vector<CardResConfig> playfieldConfigs = CardResConfig::loadPlayfieldConfigs(levelConfig);
    std::vector<CardResConfig> stackConfigs = CardResConfig::loadStackConfigs(levelConfig);
    
    // 2. 创建 CardModel 列表
    std::vector<CardModel*> playfieldCards;
    for (const auto& config : playfieldConfigs) {
        CardModel* card = createCardModelFromConfig(config, false);
        playfieldCards.push_back(card);
    }
    
    std::vector<CardModel*> stackCards;
    for (const auto& config : stackConfigs) {
        CardModel* card = createCardModelFromConfig(config, true);
        stackCards.push_back(card);
    }
    
    // 3. 创建并初始化 GameModel
    GameModel* gameModel = new GameModel();
    gameModel->init(playfieldCards, stackCards);
    
    return gameModel;
}

CardModel* GameModelFromLevelGenerator::createCardModelFromConfig(const CardResConfig& config, bool isStack)
{
    // 创建 CardModel
    CardModel* card = new CardModel(
        static_cast<CardSuitType>(config.suitImage),
        static_cast<CardFaceType>(config.bigRankImage)
    );
    
    // 设置卡牌ID（使用配置中的ID）
    card->setId(config.id);
    
    // 设置位置
    if (isStack) {
        card->setPosition(config.stackPos);
    } else {
        card->setPosition(config.playfieldPos);
    }
    
    // 设置初始状态
    card->setFaceUp(false);  // 默认背面朝上
    card->setClickable(true);
    
    return card;
}

