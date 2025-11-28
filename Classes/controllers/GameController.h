#pragma once
#include "cocos2d.h"
#include "../models/GameModel.h"
#include "../views/GameView.h"
#include "../managers/UndoManager.h"
#include "../controllers/PlayFieldController.h"
#include "../controllers/StackController.h"

USING_NS_CC;

// 游戏主控制器
class GameController {
public:
    GameController();
    ~GameController();
    
    // 启动游戏
    bool startGame(int levelId);
    
    // 设置游戏视图（由 GameScene 调用）
    void setGameView(GameView* gameView);
    
    // 撤销上一次操作
    bool undo();
    
    // 是否可撤销
    bool canUndo() const;
    
    // 记录撤销操作（在操作前调用，收集卡牌位置信息）
    // 供子Controller调用
    void recordUndo();
    
    // 获取游戏模型（供 GameScene 使用）
    GameModel* getGameModel() const { return _gameModel; }
    
private:
    // 初始化子控制器
    void initSubControllers();
    
    // 初始化游戏视图
    void initGameView();
    
private:
    GameModel* _gameModel = nullptr;           // 游戏数据模型
    GameView* _gameView = nullptr;             // 游戏视图
    UndoManager* _undoManager = nullptr;      // 撤销管理器
    
    PlayFieldController* _playFieldController = nullptr;  // 游戏区域控制器
    StackController* _stackController = nullptr;          // 堆叠区域控制器
    
    int _currentLevelId = 0;  // 当前关卡ID
};
