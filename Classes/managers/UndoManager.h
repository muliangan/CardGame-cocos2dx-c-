#pragma once
#include "../models/GameModel.h"
#include "cocos2d.h"
#include <vector>
#include <memory>
#include <map>

USING_NS_CC;

// 卡牌位置信息（世界坐标）
struct CardPositionInfo {
    int cardId;           // 卡牌ID
    Vec2 worldPosition;   // 世界坐标位置
    bool isInStack;       // 是否在Stack区域（true=Stack, false=PlayField）
};

// 每次操作的快照
struct UndoSnapshot {
    std::vector<CardModel*> playfieldCards;
    std::vector<CardModel*> stackCards;
    std::map<int, CardPositionInfo> cardPositions;  // 卡牌ID到位置的映射
};

class UndoManager {
public:
    UndoManager() = default;

    // 初始化管理器
    void init(GameModel* model);

    // 记录一次操作
    // cardPositions - 卡牌位置信息（卡牌ID到位置的映射）
    void record(const std::map<int, CardPositionInfo>& cardPositions);
    
    // 撤销上一次操作
    // 返回: 撤销的快照（包含Model状态和位置信息），如果无法撤销返回nullptr
    UndoSnapshot* undo();

    // 是否可撤销
    bool canUndo() const { return !_history.empty(); }

private:
    GameModel* _model = nullptr;
    std::vector<UndoSnapshot> _history;  // 历史记录栈
};

