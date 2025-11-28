#include "UndoManager.h"
#include "../models/GameModel.h"

void UndoManager::init(GameModel* model)
{
    _model = model;
    _history.clear();
}

void UndoManager::record(const std::map<int, CardPositionInfo>& cardPositions)
{
    if (!_model) return;

    // 记录 playfield 和 stack 的卡牌，以及卡牌位置信息
    UndoSnapshot snapshot;
    snapshot.playfieldCards = _model->getPlayfieldCards();
    snapshot.stackCards = _model->getStackCards();
    snapshot.cardPositions = cardPositions;

    _history.push_back(snapshot);
}

UndoSnapshot* UndoManager::undo()
{
    if (_history.empty() || !_model) return nullptr;

    // 取出最后一次操作
    UndoSnapshot snapshot = _history.back();
    _history.pop_back();

    // 恢复 GameModel 状态
    _model->init(snapshot.playfieldCards, snapshot.stackCards);

    // 返回快照（调用者负责释放，或者使用静态存储）
    static UndoSnapshot lastSnapshot;
    lastSnapshot = snapshot;
    return &lastSnapshot;
}

