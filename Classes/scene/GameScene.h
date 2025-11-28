#pragma once
#include "cocos2d.h"
#include "../controllers/GameController.h"

USING_NS_CC;


// 游戏场景
class GameScene : public Scene {
public:

    //创建游戏场景
    //levelNumber - 关卡编号（1, 2, 3...）
    static Scene* createScene(int levelNumber);
    
    virtual bool init() override;
    
    //初始化场景
    bool initWithLevel(int levelNumber);
    CREATE_FUNC(GameScene);
    
private:
    GameController* _gameController = nullptr;
    int _levelNumber;
};

