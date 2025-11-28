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
        CCLOG("StackController::initView: stackView is null");
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
    
    CCLOG("StackController::initView: View initialized successfully");
}

void StackController::handleCardClick(CardView* cardView)
{
    if (!cardView || !cardView->getModel() || !_gameModel || !_stackView) {
        return;
    }
    
    CardModel* clickedCardModel = cardView->getModel();
    
    CCLOG("StackController::handleCardClick: Card ID %d clicked", clickedCardModel->getId());
    
    // 检查是否是Stack区的卡牌
    if (!_gameModel->isInStack(clickedCardModel)) {
        CCLOG("StackController::handleCardClick: Card is not in stack");
        return;
    }
    
    // 检查是否已经是顶部牌
    CardModel* topCard = _gameModel->getTopStackCard();
    if (topCard == clickedCardModel) {
        CCLOG("StackController::handleCardClick: Top card clicked, no need to move");
        return;
    }
    
    //  检查是否只能替换最后一张牌（只能点击倒数第二张牌）
    CardModel* secondTopCard = _gameModel->getSecondTopStackCard();
    if (clickedCardModel != secondTopCard) {
        CCLOG("StackController::handleCardClick: Can only replace top card with second top card. Clicked card is not the second top card.");
        return;
    }
    
    // 记录撤销操作（在操作前调用，收集卡牌位置信息）
    if (_gameController) {
        _gameController->recordUndo();
    }
    
    // 保存被点击卡牌的原始位置
    Vec2 clickedCardPosition = cardView->getPosition();
    
    // 更新GameModel：将点击的卡牌移到顶部，并获取被替换的顶部牌
    CardModel* oldTopCardModel = _gameModel->moveCardToTop(clickedCardModel);
    if (!oldTopCardModel) {
        CCLOG("StackController::handleCardClick: Failed to move card to top");
        return;
    }
    
    // 找到被替换的顶部牌对应的CardView
    // 由于vector已经交换，需要找到对应的CardView
    CardView* oldTopCardView = nullptr;
    const auto& cardViews = _stackView->getCardViews();
    for (auto* cv : cardViews) {
        if (cv && cv->getModel() == oldTopCardModel) {
            oldTopCardView = cv;
            break;
        }
    }
    
    if (!oldTopCardView) {
        CCLOG("StackController::handleCardClick: Failed to find old top card view");
        return;
    }
    
    // 获取顶部位置
    Vec2 topPosition = _stackView->getTopCardPosition();
    
    // 调用View的动画接口，播放移动动画（交换两张卡牌的位置）
    _stackView->playCardMoveToTopAnimation(cardView, oldTopCardView, topPosition, clickedCardPosition);
    
    CCLOG("StackController::handleCardClick: Card ID %d moved to top position, Card ID %d moved to clicked position", 
          clickedCardModel->getId(), oldTopCardModel->getId());
    
    // 更新回退按钮状态
    if (_gameController && _stackView) {
        _stackView->updateUndoButtonState(_gameController->canUndo());
    }
}
