#include "PlayFieldController.h"
#include "../views/PlayFieldView.h"
#include "../views/StackView.h"
#include "../views/CardView.h"
#include "../models/CardModel.h"
#include "../controllers/GameController.h"
#include "cocos2d.h"
#include <cmath>

USING_NS_CC;

void PlayFieldController::init(GameModel* gameModel, UndoManager* undoManager, GameController* gameController)
{
    _gameModel = gameModel;
    _undoManager = undoManager;
    _gameController = gameController;
}

void PlayFieldController::initView(PlayFieldView* playFieldView)
{
    if (!playFieldView) {
        CCLOG("PlayFieldController::initView: playFieldView is null");
        return;
    }
    
    _playFieldView = playFieldView;
    
    // 设置卡牌点击回调
    _playFieldView->setCardClickCallback([this](int cardId) {
        CardView* cardView = _playFieldView->getCardViewById(cardId);
        if (cardView) {
            this->handleCardClick(cardView);
        }
    });
    
    CCLOG("PlayFieldController::initView: View initialized successfully");
}

void PlayFieldController::setStackView(StackView* stackView)
{
    _stackView = stackView;
}

void PlayFieldController::handleCardClick(CardView* cardView)
{
    if (!cardView || !cardView->getModel() || !_gameModel || !_stackView) {
        CCLOG("PlayFieldController::handleCardClick: Invalid parameters");
        return;
    }
    
    // 获取点击的卡牌a（CardView和CardModel）
    CardModel* cardA = cardView->getModel();
    CCLOG("PlayFieldController::handleCardClick: PlayField card A (ID %d, Rank %d) clicked", 
          cardA->getId(), static_cast<int>(cardA->getRank()));
    
    // 获取Stack最后一张牌b
    CardModel* cardB = _gameModel->getTopStackCard();
    if (!cardB) {
        CCLOG("PlayFieldController::handleCardClick: No card in stack");
        return;
    }
    CCLOG("PlayFieldController::handleCardClick: Stack top card B (ID %d, Rank %d)", 
          cardB->getId(), static_cast<int>(cardB->getRank()));
    
    //检查匹配条件：|a的点数 - b的点数| == 1
    int rankA = static_cast<int>(cardA->getRank());
    int rankB = static_cast<int>(cardB->getRank());
    int rankDiff = std::abs(rankA - rankB);
    
    CCLOG("PlayFieldController::handleCardClick: Rank difference = %d", rankDiff);
    
    if (rankDiff == 1) {
        // 匹配成功：执行替换逻辑
        CCLOG("PlayFieldController::handleCardClick: Match successful! Moving card A to replace card B");
        replaceTrayWithPlayFieldCard(cardView);
    } else {
        CCLOG("PlayFieldController::handleCardClick: Match failed! Rank difference is %d (required: 1)", rankDiff);
    }
}

void PlayFieldController::replaceTrayWithPlayFieldCard(CardView* playFieldCardView)
{
    if (!playFieldCardView || !playFieldCardView->getModel() || !_gameModel || !_stackView) {
        CCLOG("PlayFieldController::replaceTrayWithPlayFieldCard: Invalid parameters");
        return;
    }
    
    // 获取卡牌a（PlayField卡牌）
    CardModel* cardA = playFieldCardView->getModel();
    
    CCLOG("PlayFieldController::replaceTrayWithPlayFieldCard: Starting replacement process");
    CCLOG("  - Card A (PlayField): ID %d, Rank %d", cardA->getId(), static_cast<int>(cardA->getRank()));
    
    //记录撤销操作（在操作前调用，收集卡牌位置信息）
    if (_gameController) {
        _gameController->recordUndo();
    }
    
    // 更新Model数据：
    // 将卡牌a的数据替换到Stack顶部的b上（在Stack中）
    // 从PlayField中删除a的数据
    CardModel* cardB = _gameModel->movePlayFieldCardToStackTop(cardA);
    if (!cardB) {
        CCLOG("PlayFieldController::replaceTrayWithPlayFieldCard: Failed to move card in model");
        return;
    }
    CCLOG("  - Card B (Stack top, replaced): ID %d, Rank %d", cardB->getId(), static_cast<int>(cardB->getRank()));
    
    // 找到原来的顶部牌b对应的CardView（用于动画和移除）
    CardView* oldTopCardView = nullptr;
    const auto& stackCardViews = _stackView->getCardViews();
    for (auto* cv : stackCardViews) {
        if (cv && cv->getModel() == cardB) {
            oldTopCardView = cv;
            break;
        }
    }
    
    if (!oldTopCardView) {
        CCLOG("PlayFieldController::replaceTrayWithPlayFieldCard: Warning: Could not find CardView for card B");
    }
    
    // 获取Stack顶部位置（b的位置）
    Vec2 topPosition = _stackView->getTopCardPosition();
    CCLOG("  - Stack top position: (%.1f, %.1f)", topPosition.x, topPosition.y);
    
    // 播放动画：将卡牌a移动到Stack区域b的位置
    _stackView->playPlayFieldCardMoveToStackAnimation(playFieldCardView, oldTopCardView, topPosition, _playFieldView);
    
    CCLOG("PlayFieldController::replaceTrayWithPlayFieldCard: Replacement complete");
    CCLOG("  - Card A (ID %d) moved to Stack top position", cardA->getId());
    CCLOG("  - Card B (ID %d) will be removed", cardB->getId());
    
    // 更新回退按钮状态
    if (_gameController && _stackView) {
        _stackView->updateUndoButtonState(_gameController->canUndo());
    }
}
