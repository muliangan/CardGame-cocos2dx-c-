#include "CardTestScene.h"
#include "../controllers/GameController.h"
#include "../views/GameView.h"
#include "../models/GameModel.h"

USING_NS_CC;

Scene* CardTestScene::createScene(int levelId)
{
    auto scene = CardTestScene::create();
    if (scene) {
        scene->setLevelId(levelId);
    }
    return scene;
}

bool CardTestScene::init()
{
    if (!Scene::init())
        return false;
    
    // 使用传入的关卡编号
    int levelId = _levelId;
    CCLOG("CardTestScene::init: Starting test scene initialization with level %d", levelId);
    
    // 1. 创建游戏控制器
    _gameController = new GameController();
    
    CCLOG("CardTestScene::init: GameController created");
    
    // 2. 调用 GameController::startGame(levelId = 1)
    // 这会自动执行：
    //    - LevelConfigLoader::loadLevelConfig(levelId) 获取 LevelConfig
    //    - GameModelFromLevelGenerator::generateGameModel 生成 GameModel
    //    - 初始化各子控制器：
    //        - PlayFieldController::init(...)
    //        - StackController::init(...)
    //        - UndoManager::init(...)
    CCLOG("CardTestScene::init: Calling GameController::startGame(%d)", levelId);
    if (!_gameController->startGame(levelId)) {
        CCLOG("CardTestScene::init: Failed to start game with level %d", levelId);
        return false;
    }
    CCLOG("CardTestScene::init: GameController::startGame completed successfully");
    
    // 3. 创建 GameView 并添加到父节点
    GameModel* gameModel = _gameController->getGameModel();
    if (!gameModel) {
        CCLOG("CardTestScene::init: GameModel is null after startGame");
        return false;
    }
    
    CCLOG("CardTestScene::init: Creating GameView with GameModel");
    GameView* gameView = GameView::create(gameModel);
    if (!gameView) {
        CCLOG("CardTestScene::init: Failed to create GameView");
        return false;
    }
    
    // 设置 GameView 的位置和大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    gameView->setPosition(origin);
    gameView->setContentSize(visibleSize);
    
    // 添加到场景
    this->addChild(gameView, 1);
    CCLOG("CardTestScene::init: GameView added to scene");
    
    // 4. 设置 GameView 到 GameController
    // 这会自动执行：
    //    - 初始化各子控制器的视图：
    //        - StackController::initView(...)
    //        - PlayFieldController::initView(...)
    //    - GameView::initUI（设置初始布局）
    CCLOG("CardTestScene::init: Setting GameView to GameController");
    _gameController->setGameView(gameView);
    CCLOG("CardTestScene::init: GameView set to GameController, views initialized");
    
    // 5. 入场动画不实现（按用户要求）
    
    CCLOG("CardTestScene::init: Test scene initialized successfully!");
    CCLOG("CardTestScene::init: Level %d loaded with %d PlayField cards and %d Stack cards",
        levelId,
        static_cast<int>(gameModel->getPlayfieldCards().size()),
        static_cast<int>(gameModel->getStackCards().size()));
    CCLOG("CardTestScene::init: Click on cards to test game functionality");
    
    return true;
}