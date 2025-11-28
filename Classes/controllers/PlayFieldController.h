#pragma once
#include "../models/GameModel.h"
#include "../managers/UndoManager.h"

class PlayFieldView;
class CardView;

// 游戏区域控制器
class PlayFieldController {
public:
    PlayFieldController() = default;
    ~PlayFieldController() = default;
    
    // 初始化控制器
    // gameModel - 游戏数据模型
    // undoManager - 撤销管理器
    // gameController - 游戏主控制器（用于记录撤销操作）
    void init(GameModel* gameModel, UndoManager* undoManager, class GameController* gameController = nullptr);
    
    // 初始化视图
    void initView(PlayFieldView* playFieldView);
    
    // 处理卡牌点击事件
    void handleCardClick(CardView* cardView);
    
    // 设置StackView引用（用于播放动画）
    // stackView - Stack区域视图
    void setStackView(class StackView* stackView);
    
private:
    // 用PlayField卡牌替换Stack顶部牌
    void replaceTrayWithPlayFieldCard(CardView* playFieldCardView);
    
private:
    GameModel* _gameModel = nullptr;
    UndoManager* _undoManager = nullptr;
    PlayFieldView* _playFieldView = nullptr;
    class StackView* _stackView = nullptr;
    class GameController* _gameController = nullptr;  // 用于记录撤销操作
};
