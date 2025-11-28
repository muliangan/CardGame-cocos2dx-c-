#include "StackController.h"
#include "../views/StackView.h"
#include "../views/CardView.h"
#include "../models/CardModel.h"
#include "../controllers/GameController.h"
#include "cocos2d.h"

USING_NS_CC;

void StackController::init(GameModel* gameModel, UndoManager* undoManager, GameController* gameController)
{
    _gameModel = gameModel;
    _undoManager = undoManager;
    _gameController = gameController;
}

void StackController::initView(StackView* stackView)
{
    if (!stackView) {
        return;
    }
    
    _stackView = stackView;
    
    // 设置卡牌点击回调
    _stackView->setCardClickCallback([this](int cardId) {
        CardView* cardView = _stackView->getCardViewById(cardId);
        if (cardView) {
            this->handleCardClick(cardView);
        }
    });
}

void StackController::handleCardClick(CardView* cardView)
{
    if (!cardView || !cardView->getModel() || !_gameModel || !_stackView) {
        return;
    }
    
    CardModel* clickedCardModel = cardView->getModel();
    
    // 检查是否是Stack区的卡牌
    if (!_gameModel->isInStack(clickedCardModel)) {
        return;
    }
    
    // 检查是否已经是顶部牌（最后一张牌）
    CardModel* topCard = _gameModel->getTopStackCard();
    if (topCard == clickedCardModel) {
        return;
    }
    
    // 允许点击任意非最后一张的牌，将其替换到最后一张的位置
    
    // 记录撤销操作（在操作前调用，收集卡牌位置信息）
    if (_gameController) {
        _gameController->recordUndo();
    }
    
    // 更新GameModel：将点击的卡牌移到顶部，并获取被替换的顶部牌
    CardModel* oldTopCardModel = _gameModel->moveCardToTop(clickedCardModel);
    if (!oldTopCardModel) {
        return;
    }
    
    // 找到被替换的顶部牌对应的CardView（在更新Model之前查找，因为之后oldTopCardModel会被移除）
    CardView* oldTopCardView = nullptr;
    const auto& cardViews = _stackView->getCardViews();
    for (auto* cv : cardViews) {
        if (cv && cv->getModel() == oldTopCardModel) {
            oldTopCardView = cv;
            break;
        }
    }
    
    if (!oldTopCardView) {
        return;
    }
    
    // 获取顶部位置
    Vec2 topPosition = _stackView->getTopCardPosition();
    
    // 调用View的动画接口，播放移动动画（点击的牌a移动到顶部，原来的顶部牌b直接消失）
    _stackView->playCardMoveToTopAnimation(cardView, oldTopCardView, topPosition);
    
    // 更新回退按钮状态
    if (_gameController && _stackView) {
        _stackView->updateUndoButtonState(_gameController->canUndo());
    }
}
