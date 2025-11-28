#pragma once
#include "../models/GameModel.h"
#include "../managers/UndoManager.h"

class StackView;
class CardView;

// 堆叠区域控制器
class StackController {
public:
    StackController() = default;
    ~StackController() = default;
    
    // 初始化控制器
    // gameModel - 游戏数据模型
    // undoManager - 撤销管理器
    // gameController - 游戏主控制器（用于记录撤销操作）
    void init(GameModel* gameModel, UndoManager* undoManager, class GameController* gameController = nullptr);
    
    // 初始化视图
    void initView(StackView* stackView);
    
    // 处理卡牌点击事件
    void handleCardClick(CardView* cardView);
    
private:
    GameModel* _gameModel = nullptr;
    UndoManager* _undoManager = nullptr;
    StackView* _stackView = nullptr;
    class GameController* _gameController = nullptr;  // 用于记录撤销操作
};
