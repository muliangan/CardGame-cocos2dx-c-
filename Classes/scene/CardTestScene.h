#pragma once
#include "cocos2d.h"
#include "../controllers/GameController.h"

USING_NS_CC;

// 卡牌测试场景
// 职责：测试游戏控制器和视图的完整流程
class CardTestScene : public Scene
{
public:
    static Scene* createScene(int levelId = 1);
    virtual bool init();
    void setLevelId(int levelId) { _levelId = levelId; }

    // implement the "static create()" method manually
    CREATE_FUNC(CardTestScene);
    
private:
    GameController* _gameController = nullptr;  // 游戏控制器
    int _levelId = 1;  // 关卡编号
};