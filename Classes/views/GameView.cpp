#include "GameView.h"
#include "../views/CardView.h"
#include "cocos2d.h"

USING_NS_CC;

GameView* GameView::create(GameModel* model)
{
    GameView* ret = new GameView();
    if (ret && ret->init(model)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool GameView::init(GameModel* model)
{
    if (!Node::init()) {
        return false;
    }
    
    _gameModel = model;
    
    // 设置 anchor point 为 (0, 0)，确保坐标系统一致
    setAnchorPoint(Vec2::ZERO);
    
    // 不设置 ContentSize，让 Node 自动计算，避免裁剪子节点

    // 不注册触摸事件监听器，让子视图（StackView和PlayFieldView）自己处理触摸事件
    // 这样可以避免父节点拦截子节点的触摸事件

    return true;
}

void GameView::initUI(PlayFieldView* playFieldView, StackView* stackView)
{
    // 保存视图引用
    _playFieldView = playFieldView;
    _stackView = stackView;
    
    this->addChild(playFieldView, 0);
    this->addChild(stackView, 0);

    // PlayField cards
    // CardResConfig中的playfieldPos是JSON中的位置（左下角，y已加580），需要转换为世界坐标（中心点）
    for (auto cardModel : _gameModel->getPlayfieldCards()) {
        auto cardView = CardView::create(cardModel);
        Vec2 jsonPos = cardModel->getPosition();  // CardResConfig中的playfieldPos（左下角，y已加580）
        Size cardSize = cardView->getContentSize();  // 获取CardView实际大小
        
        // 将JSON位置（左下角）转换为世界坐标（中心点）
        // 世界坐标系：窗口左下角为原点(0,0)
        // 中心点 = 左下角 + (width/2, height/2)
        Vec2 worldPos = Vec2(jsonPos.x + cardSize.width / 2.0f, jsonPos.y + cardSize.height / 2.0f);
        
        // 将世界坐标转换为相对于PlayFieldView的本地坐标
        Vec2 localPos = playFieldView->convertToNodeSpace(worldPos);
        cardView->setPosition(localPos);
        playFieldView->addCardView(cardView);
        _fieldCardViews.push_back(cardView);  // 保存到列表以便动画使用
    }

    // Stack cards
    // CardResConfig中的stackPos已经是世界坐标（中心点）
    for (auto cardModel : _gameModel->getStackCards()) {
        auto cardView = CardView::create(cardModel);
        Vec2 worldPos = cardModel->getPosition();  // CardResConfig中的stackPos（世界坐标）
        
        // 将世界坐标转换为相对于StackView的本地坐标
        Vec2 localPos = stackView->convertToNodeSpace(worldPos);
        cardView->setPosition(localPos);
        stackView->addCardView(cardView);
        _stackCardViews.push_back(cardView);  // 保存到列表以便动画使用
    }
}

void GameView::addCardView(CardView* cardView, bool isStack)
{
    this->addChild(cardView);

    if (isStack)
        _stackCardViews.push_back(cardView);
    else
        _fieldCardViews.push_back(cardView);
}
void GameView::setCardClickCallback(const std::function<void(CardView*)>& callback)
{
    _onCardClickCallback = callback;
}
bool GameView::onTouchBegan(Touch* touch, Event* event)
{
    // 先检查触摸位置是否在StackView区域内
    // 如果在StackView区域内，让StackView自己处理
    if (_stackView) {
        Vec2 stackLocalPos = _stackView->convertToNodeSpace(touch->getLocation());
        Size stackSize = _stackView->getContentSize();
        if (stackLocalPos.x >= 0 && stackLocalPos.x <= stackSize.width &&
            stackLocalPos.y >= 0 && stackLocalPos.y <= stackSize.height) {
            // 触摸在StackView区域内，不处理，让StackView处理
            return false;
        }
    }
    
    // 检查触摸位置是否在PlayFieldView区域内
    if (_playFieldView) {
        Vec2 fieldLocalPos = _playFieldView->convertToNodeSpace(touch->getLocation());
        Size fieldSize = _playFieldView->getContentSize();
        if (fieldLocalPos.x >= 0 && fieldLocalPos.x <= fieldSize.width &&
            fieldLocalPos.y >= 0 && fieldLocalPos.y <= fieldSize.height) {
            // 触摸在PlayFieldView区域内，不处理，让PlayFieldView处理
            return false;
        }
    }
    
    // 如果不在任何子视图区域内，才处理
    Vec2 location = this->convertToNodeSpace(touch->getLocation());
    CardView* card = findTopCard(location);
    if (card && _onCardClickCallback)
    {
        _onCardClickCallback(card);
        return true;
    }
    return false;
}
CardView* GameView::findTopCard(const Vec2& pos)
{
    // 先判断 stack 卡牌，因为它在最上层
    for (auto it = _stackCardViews.rbegin(); it != _stackCardViews.rend(); ++it)
    {
        if ((*it)->getBoundingBox().containsPoint(pos))
            return *it;
    }

    // 再判断桌面卡牌
    for (auto it = _fieldCardViews.rbegin(); it != _fieldCardViews.rend(); ++it)
    {
        if ((*it)->getBoundingBox().containsPoint(pos))
            return *it;
    }

    return nullptr;
}

void GameView::playEntranceAnimation()
{
    // 动画参数
    float animationDuration = 0.3f;  // 每张卡牌的动画时长
    float delayBetweenCards = 0.05f;  // 卡牌之间的延迟
    float startScale = 0.0f;          // 起始缩放
    float endScale = 1.0f;            // 结束缩放
    GLubyte startOpacity = 0;         // 起始透明度
    GLubyte endOpacity = 255;         // 结束透明度
    
    // 播放 PlayField 卡牌的入场动画
    float currentDelay = 0.0f;
    for (size_t i = 0; i < _fieldCardViews.size(); ++i) {
        CardView* cardView = _fieldCardViews[i];
        if (cardView) {
            // 设置初始状态（透明且缩放为0）
            cardView->setScale(startScale);
            cardView->setOpacity(startOpacity);
            
            // 创建动画序列：延迟 -> 缩放 + 淡入
            auto delay = DelayTime::create(currentDelay);
            auto scaleTo = ScaleTo::create(animationDuration, endScale);
            auto fadeIn = FadeIn::create(animationDuration);
            auto spawn = Spawn::create(scaleTo, fadeIn, nullptr);
            auto sequence = Sequence::create(delay, spawn, nullptr);
            
            cardView->runAction(sequence);
            
            currentDelay += delayBetweenCards;
        }
    }
    
    // 播放 Stack 卡牌的入场动画
    currentDelay = 0.0f;
    for (size_t i = 0; i < _stackCardViews.size(); ++i) {
        CardView* cardView = _stackCardViews[i];
        if (cardView) {
            // 设置初始状态（透明且缩放为0）
            cardView->setScale(startScale);
            cardView->setOpacity(startOpacity);
            
            // 创建动画序列：延迟 -> 缩放 + 淡入
            auto delay = DelayTime::create(currentDelay);
            auto scaleTo = ScaleTo::create(animationDuration, endScale);
            auto fadeIn = FadeIn::create(animationDuration);
            auto spawn = Spawn::create(scaleTo, fadeIn, nullptr);
            auto sequence = Sequence::create(delay, spawn, nullptr);
            
            cardView->runAction(sequence);
            
            currentDelay += delayBetweenCards;
        }
    }
}

std::map<int, CardPositionInfo> GameView::collectCardPositions() const
{
    std::map<int, CardPositionInfo> positions;
    
    // 收集PlayField区域的卡牌位置
    if (_playFieldView) {
        const auto& cardViews = _playFieldView->getCardViews();
        for (auto* cardView : cardViews) {
            if (cardView && cardView->getModel()) {
                CardPositionInfo info;
                info.cardId = cardView->getModel()->getId();
                // 将本地坐标转换为世界坐标
                Vec2 localPos = cardView->getPosition();
                info.worldPosition = _playFieldView->convertToWorldSpace(localPos);
                info.isInStack = false;
                positions[info.cardId] = info;
            }
        }
    }
    
    // 收集Stack区域的卡牌位置
    if (_stackView) {
        const auto& cardViews = _stackView->getCardViews();
        for (auto* cardView : cardViews) {
            if (cardView && cardView->getModel()) {
                CardPositionInfo info;
                info.cardId = cardView->getModel()->getId();
                // 将本地坐标转换为世界坐标
                Vec2 localPos = cardView->getPosition();
                info.worldPosition = _stackView->convertToWorldSpace(localPos);
                info.isInStack = true;
                positions[info.cardId] = info;
            }
        }
    }
    
    return positions;
}

CardView* GameView::findCardViewByModelId(int cardId) const
{
    // 在PlayField区域查找
    if (_playFieldView) {
        const auto& cardViews = _playFieldView->getCardViews();
        for (auto* cardView : cardViews) {
            if (cardView && cardView->getModel() && cardView->getModel()->getId() == cardId) {
                return cardView;
            }
        }
    }
    
    // 在Stack区域查找
    if (_stackView) {
        const auto& cardViews = _stackView->getCardViews();
        for (auto* cardView : cardViews) {
            if (cardView && cardView->getModel() && cardView->getModel()->getId() == cardId) {
                return cardView;
            }
        }
    }
    
    return nullptr;
}

void GameView::playUndoAnimation(CardView* cardView, const Vec2& targetWorldPos, bool isInStack)
{
    if (!cardView) {
        return;
    }
    
    // 获取当前父节点
    Node* currentParent = cardView->getParent();
    if (!currentParent) {
        return;
    }
    
    // 确定目标父节点
    Node* targetParent = nullptr;
    if (isInStack) {
        targetParent = _stackView;
    } else {
        targetParent = _playFieldView;
    }
    if (!targetParent) {
        return;
    }
    
    // 获取当前世界坐标位置
    Vec2 currentWorldPos = currentParent->convertToWorldSpace(cardView->getPosition());
    
    // 如果父节点不同，需要先移动到目标父节点
    if (currentParent != targetParent) {
        // 保留引用
        cardView->retain();
        // 从当前父节点移除
        cardView->removeFromParent();
        
        // 更新View的内部列表（在添加到新父节点之前）
        if (isInStack) {
            // 从PlayFieldView列表移除，添加到StackView列表
            if (_playFieldView) {
                _playFieldView->removeCardView(cardView);
            }
            // 添加到StackView列表
            if (_stackView) {
                _stackView->addCardViewToList(cardView);
            }
            // 更新GameView的内部列表
            auto fieldIt = std::find(_fieldCardViews.begin(), _fieldCardViews.end(), cardView);
            if (fieldIt != _fieldCardViews.end()) {
                _fieldCardViews.erase(fieldIt);
            }
            auto stackIt = std::find(_stackCardViews.begin(), _stackCardViews.end(), cardView);
            if (stackIt == _stackCardViews.end()) {
                _stackCardViews.push_back(cardView);
            }
        } else {
            // 从StackView列表移除，添加到PlayFieldView列表
            if (_stackView) {
                _stackView->removeCardView(cardView);
            }
            // 添加到PlayFieldView列表
            if (_playFieldView) {
                _playFieldView->addCardViewToList(cardView);
            }
            // 更新GameView的内部列表
            auto stackIt = std::find(_stackCardViews.begin(), _stackCardViews.end(), cardView);
            if (stackIt != _stackCardViews.end()) {
                _stackCardViews.erase(stackIt);
            }
            auto fieldIt = std::find(_fieldCardViews.begin(), _fieldCardViews.end(), cardView);
            if (fieldIt == _fieldCardViews.end()) {
                _fieldCardViews.push_back(cardView);
            }
        }
        
        // 添加到目标父节点
        targetParent->addChild(cardView);
        // 转换位置到目标父节点的本地坐标
        Vec2 targetLocalPos = targetParent->convertToNodeSpace(currentWorldPos);
        cardView->setPosition(targetLocalPos);
        // 释放引用
        cardView->release();
    }
    
    // 计算目标本地坐标
    Vec2 targetLocalPos = targetParent->convertToNodeSpace(targetWorldPos);
    
    // 播放移动动画
    float moveDuration = 0.3f;
    auto moveAction = MoveTo::create(moveDuration, targetLocalPos);
    cardView->runAction(moveAction);
}


