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
}

void PlayFieldController::setStackView(StackView* stackView)
{
    _stackView = stackView;
}

void PlayFieldController::handleCardClick(CardView* cardView)
{
    if (!cardView || !cardView->getModel() || !_gameModel || !_stackView) {
        return;
    }
    
    // 获取点击的卡牌a（CardView和CardModel）
    CardModel* cardA = cardView->getModel();
    
    // 获取Stack最后一张牌b
    CardModel* cardB = _gameModel->getTopStackCard();
    if (!cardB) {
        return;
    }
    
    // 检查匹配条件：使用GameModel的canMatch方法
    // 匹配规则：1. 点数差为1  2. Ace和King可以互相匹配
    bool canMatch = _gameModel->canMatch(cardA);
    
    if (canMatch) {
        // 匹配成功：执行替换逻辑
        replaceTrayWithPlayFieldCard(cardView);
    }
}

void PlayFieldController::replaceTrayWithPlayFieldCard(CardView* playFieldCardView)
{
    if (!playFieldCardView || !playFieldCardView->getModel() || !_gameModel || !_stackView) {
        return;
    }
    
    // 获取卡牌a（PlayField卡牌）
    CardModel* cardA = playFieldCardView->getModel();
    
    //记录撤销操作（在操作前调用，收集卡牌位置信息）
    if (_gameController) {
        _gameController->recordUndo();
    }
    
    // 更新Model数据：
    // 将卡牌a的数据替换到Stack顶部的b上（在Stack中）
    // 从PlayField中删除a的数据
    CardModel* cardB = _gameModel->movePlayFieldCardToStackTop(cardA);
    if (!cardB) {
        return;
    }
    
    // 找到原来的顶部牌b对应的CardView（用于动画和移除）
    CardView* oldTopCardView = nullptr;
    const auto& stackCardViews = _stackView->getCardViews();
    for (auto* cv : stackCardViews) {
        if (cv && cv->getModel() == cardB) {
            oldTopCardView = cv;
            break;
        }
    }
    
    // 获取Stack顶部位置（b的位置）
    Vec2 topPosition = _stackView->getTopCardPosition();
    
    // 播放动画：将卡牌a移动到Stack区域b的位置
    _stackView->playPlayFieldCardMoveToStackAnimation(playFieldCardView, oldTopCardView, topPosition, _playFieldView);
    
    // 更新回退按钮状态
    if (_gameController && _stackView) {
        _stackView->updateUndoButtonState(_gameController->canUndo());
    }
}
