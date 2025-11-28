#include "GameScene.h"
#include "../controllers/GameController.h"
#include "../views/GameView.h"
#include "../models/GameModel.h"

USING_NS_CC;

Scene* GameScene::createScene(int levelNumber)
{
    auto scene = GameScene::create();
    if (scene && scene->initWithLevel(levelNumber)) {
        return scene;
    }
    return nullptr;
}

bool GameScene::init()
{
    if (!Scene::init()) {
        return false;
    }
    return true;
}

bool GameScene::initWithLevel(int levelNumber)
{
    if (!init()) {
        return false;
    }
    
    _levelNumber = levelNumber;
    
    CCLOG("GameScene::initWithLevel: Initializing level %d", levelNumber);
    
    // 1. 创建游戏控制器
    _gameController = new GameController();
    
    // 2. 启动游戏（这会加载配置、生成 GameModel、初始化子控制器）
    if (!_gameController->startGame(levelNumber)) {
        CCLOG("GameScene::initWithLevel: Failed to start game");
        return false;
    }
    
    // 3. 创建 GameView 并添加到父节点
    GameModel* gameModel = _gameController->getGameModel();
    if (!gameModel) {
        CCLOG("GameScene::initWithLevel: GameModel is null");
        return false;
    }
    
    GameView* gameView = GameView::create(gameModel);
    if (!gameView) {
        CCLOG("GameScene::initWithLevel: Failed to create GameView");
        return false;
    }
    
    // 设置 GameView 的位置和大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    gameView->setPosition(origin);
    gameView->setContentSize(visibleSize);
    
    // 添加到场景
    this->addChild(gameView, 1);
    
    //设置 GameView 到 GameController（这会初始化视图和子控制器的视图）
    _gameController->setGameView(gameView);
    
    //播放入场动画
    gameView->playEntranceAnimation();
    
    CCLOG("GameScene::initWithLevel: Level %d initialized successfully", levelNumber);
    
    return true;
}
