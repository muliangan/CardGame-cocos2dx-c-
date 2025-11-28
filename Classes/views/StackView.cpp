#include "StackView.h"
#include "CardView.h"
#include "PlayFieldView.h"
#include "cocos2d.h"
#include <algorithm>

USING_NS_CC;

StackView* StackView::create()
{
    StackView* ret = new StackView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool StackView::init()
{
    if (!Node::init()) {
        return false;
    }
    
    // 设置锚点为左下角 (0, 0)，确保坐标系统一致
    setAnchorPoint(Vec2::ZERO);
    
    // 创建背景层（默认浅绿色）
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _backgroundLayer = LayerColor::create(Color4B(200, 255, 200, 255), visibleSize.width, visibleSize.height);
    if (_backgroundLayer) {
        _backgroundLayer->setAnchorPoint(Vec2::ZERO);
        _backgroundLayer->setPosition(Vec2::ZERO);
        this->addChild(_backgroundLayer, -1); // 背景层在最底层
    }
    
    // 注册触摸事件监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(StackView::onTouchBegan, this);
    listener->setSwallowTouches(true); // 吞噬触摸事件，确保StackView优先处理
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    // 创建回退按钮
    createUndoButton();
    
    CCLOG("StackView initialized successfully");
    return true;
}

void StackView::addCardView(CardView* cardView)
{
    if (!cardView) {
        CCLOG("StackView::addCardView: cardView is null");
        return;
    }
    
    // 分配卡牌ID
    int cardId = _nextCardId++;
    _cardIdMap[cardView] = cardId;
    _cardViews.push_back(cardView);
    
    // 将卡牌视图添加到节点树
    this->addChild(cardView);
    
    CCLOG("StackView::addCardView: Added card with ID %d", cardId);
}

void StackView::setCardClickCallback(const std::function<void(int cardId)>& callback)
{
    _onCardClickCallback = callback;
}

void StackView::setBackgroundColor(const Color4B& color)
{
    if (_backgroundLayer) {
        // LayerColor 没有直接设置颜色的方法，需要重新创建
        // 使用当前背景层的大小
        Size layerSize = _backgroundLayer->getContentSize();
        auto oldLayer = _backgroundLayer;
        
        _backgroundLayer = LayerColor::create(color, layerSize.width, layerSize.height);
        _backgroundLayer->setAnchorPoint(Vec2::ZERO);
        _backgroundLayer->setPosition(Vec2::ZERO);
        
        // 替换背景层
        this->removeChild(oldLayer);
        this->addChild(_backgroundLayer, -1);
    } else {
        // 如果背景层不存在，创建一个新的
        auto visibleSize = Director::getInstance()->getVisibleSize();
        _backgroundLayer = LayerColor::create(color, visibleSize.width, visibleSize.height);
        _backgroundLayer->setAnchorPoint(Vec2::ZERO);
        _backgroundLayer->setPosition(Vec2::ZERO);
        this->addChild(_backgroundLayer, -1);
    }
}

void StackView::setBackgroundColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    setBackgroundColor(Color4B(r, g, b, a));
}

void StackView::setContentSize(const Size& size)
{
    Node::setContentSize(size);
    
    // 更新背景层大小
    if (_backgroundLayer) {
        _backgroundLayer->changeWidthAndHeight(size.width, size.height);
    } else {
        // 如果背景层不存在，创建一个新的
        _backgroundLayer = LayerColor::create(Color4B(200, 255, 200, 255), size.width, size.height);
        _backgroundLayer->setAnchorPoint(Vec2::ZERO);
        _backgroundLayer->setPosition(Vec2::ZERO);
        this->addChild(_backgroundLayer, -1);
    }
    
    // 更新回退按钮位置（如果按钮已创建）
    if (_undoButton) {
        Size buttonSize = _undoButton->getContentSize();
        Vec2 buttonPos(size.width - buttonSize.width / 2 - 20, size.height - buttonSize.height / 2 - 20);
        _undoButton->setPosition(buttonPos);
        CCLOG("StackView::setContentSize: Updated undo button position to (%.1f, %.1f)", buttonPos.x, buttonPos.y);
    }
}

bool StackView::onTouchBegan(Touch* touch, Event* event)
{
    if (!touch) {
        return false;
    }
    
    // 获取触摸位置（世界坐标）
    Vec2 worldPos = touch->getLocation();
    CCLOG("StackView::onTouchBegan: Touch detected at world pos (%.1f, %.1f)", worldPos.x, worldPos.y);
    
    // 查找最上层的卡牌（使用世界坐标）
    CardView* topCard = findTopCard(worldPos);
    
    if (topCard) {
        // 获取卡牌ID
        int cardId = getCardId(topCard);
        CCLOG("StackView::onTouchBegan: Found card with ID %d", cardId);
        
        if (cardId >= 0 && _onCardClickCallback) {
            // 调用回调函数
            CCLOG("StackView::onTouchBegan: Calling callback for card ID %d", cardId);
            _onCardClickCallback(cardId);
            return true;
        } else {
            CCLOG("StackView::onTouchBegan: Callback not set or invalid card ID");
        }
    } else {
        CCLOG("StackView::onTouchBegan: No card found at touch location");
    }
    
    return false;
}

CardView* StackView::findTopCard(const Vec2& worldPos)
{
    // 从后往前遍历（最后添加的在最上层）
    int cardIndex = 0;
    for (auto it = _cardViews.rbegin(); it != _cardViews.rend(); ++it, ++cardIndex) {
        CardView* cardView = *it;
        if (cardView && cardView->isVisible()) {
            // 获取CardView的世界坐标位置（中心点）
            Vec2 cardWorldPos = this->convertToWorldSpace(cardView->getPosition());
            Size cardSize = cardView->getContentSize();  // 卡牌大小
            
            // CardView锚点是中心点(0.5, 0.5)，在世界坐标系中计算边界框：
            float worldLeft = cardWorldPos.x - cardSize.width / 2.0f;   // 左上角X
            float worldTop = cardWorldPos.y + cardSize.height / 2.0f;   // 左上角Y
            float worldRight = cardWorldPos.x + cardSize.width / 2.0f;  // 右下角X
            float worldBottom = cardWorldPos.y - cardSize.height / 2.0f; // 右下角Y
            
            // 检查触摸点是否在卡牌的边界框内（使用世界坐标）
            if (worldPos.x >= worldLeft && worldPos.x <= worldRight &&
                worldPos.y >= worldBottom && worldPos.y <= worldTop) {
                CCLOG("StackView::findTopCard: Found matching card at index %d", cardIndex);
                return cardView;
            }
        }
    }
    
    CCLOG("StackView::findTopCard: No card found at position");
    return nullptr;
}

int StackView::getCardId(CardView* cardView) const
{
    if (!cardView) {
        return -1;
    }
    
    auto it = _cardIdMap.find(cardView);
    if (it != _cardIdMap.end()) {
        return it->second;
    }
    
    return -1;
}

CardView* StackView::getCardViewById(int cardId) const
{
    for (const auto& pair : _cardIdMap) {
        if (pair.second == cardId) {
            return pair.first;
        }
    }
    return nullptr;
}

void StackView::removeCardView(CardView* cardView)
{
    if (!cardView) {
        return;
    }
    
    // 从列表中移除
    auto it = std::find(_cardViews.begin(), _cardViews.end(), cardView);
    if (it != _cardViews.end()) {
        _cardViews.erase(it);
    }
    
    // 从映射中移除
    _cardIdMap.erase(cardView);
    
    // 从节点树移除（如果父节点是当前View）
    if (cardView->getParent() == this) {
        cardView->removeFromParent();
    }
    
    CCLOG("StackView::removeCardView: Removed card view");
}

void StackView::addCardViewToList(CardView* cardView)
{
    if (!cardView) {
        return;
    }
    
    // 检查是否已经在列表中
    auto it = std::find(_cardViews.begin(), _cardViews.end(), cardView);
    if (it == _cardViews.end()) {
        _cardViews.push_back(cardView);
        // 分配卡牌ID
        int cardId = _nextCardId++;
        _cardIdMap[cardView] = cardId;
        CCLOG("StackView::addCardViewToList: Added card to list with ID %d", cardId);
    }
}

Vec2 StackView::getTopCardPosition() const
{
    // 根据CardResConfig的逻辑，最后一张牌（顶部牌）放在中间线右边，距离200
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float screenCenterX = visibleSize.width / 2.0f;  // 屏幕中心X（中间线）
    float topCardOffset = 200.0f;  // 顶部牌距离中间线的距离
    float playfieldHeight = 1500.0f;
    float stackHeight = visibleSize.height - playfieldHeight;
    float baseY = stackHeight / 2.0f;  // Stack 区域垂直居中
    return Vec2(screenCenterX + topCardOffset, baseY);
}

void StackView::playCardMoveToTopAnimation(CardView* clickedCardView, CardView* oldTopCardView,
                                           const Vec2& topPosition, const Vec2& clickedCardPosition)
{
    if (!clickedCardView || !oldTopCardView) {
        return;
    }
    
    // 交换vector中的位置：将被点击的卡牌移到最上层（Z轴顺序）
    auto clickedIt = std::find(_cardViews.begin(), _cardViews.end(), clickedCardView);
    auto topIt = std::find(_cardViews.begin(), _cardViews.end(), oldTopCardView);
    
    if (clickedIt != _cardViews.end() && topIt != _cardViews.end()) {
        // 交换vector中的位置
        std::swap(*clickedIt, *topIt);
        // 更新Z轴顺序：被点击的卡牌在最上层
        clickedCardView->setLocalZOrder(_cardViews.size());
        oldTopCardView->setLocalZOrder(_cardViews.size() - 1);
    }
    
    // 播放移动动画：两张卡牌同时移动
    float moveDuration = 0.3f;  // 动画时长
    
    // 被点击的卡牌移动到顶部位置
    auto clickedMoveAction = MoveTo::create(moveDuration, topPosition);
    auto clickedCallback = CallFunc::create([clickedCardView, topPosition]() {
        clickedCardView->setPosition(topPosition);
        CCLOG("StackView::playCardMoveToTopAnimation: Clicked card moved to top position (%.1f, %.1f)", 
              topPosition.x, topPosition.y);
    });
    auto clickedSequence = Sequence::create(clickedMoveAction, clickedCallback, nullptr);
    clickedCardView->runAction(clickedSequence);
    
    // 原来的顶部卡牌移动到被点击卡牌的原始位置
    auto oldTopMoveAction = MoveTo::create(moveDuration, clickedCardPosition);
    auto oldTopCallback = CallFunc::create([oldTopCardView, clickedCardPosition]() {
        oldTopCardView->setPosition(clickedCardPosition);
        CCLOG("StackView::playCardMoveToTopAnimation: Old top card moved to clicked card position (%.1f, %.1f)", 
              clickedCardPosition.x, clickedCardPosition.y);
    });
    auto oldTopSequence = Sequence::create(oldTopMoveAction, oldTopCallback, nullptr);
    oldTopCardView->runAction(oldTopSequence);
}

void StackView::playPlayFieldCardMoveToStackAnimation(CardView* playFieldCardView, CardView* oldTopCardView, 
                                                      const Vec2& topPosition, PlayFieldView* playFieldView)
{
    if (!playFieldCardView) {
        return;
    }
    
    // 保存卡牌a的原始位置
    Vec2 cardAPosition = playFieldCardView->getPosition();
    Node* cardAParent = playFieldCardView->getParent();
    
    // 1. 先从PlayFieldView的列表和节点树移除卡牌a（避免parent冲突）
    if (playFieldView && cardAParent == playFieldView) {
        playFieldCardView->retain();  // 保留引用，防止被释放
        
        // 从PlayFieldView的列表中移除（但不调用removeCardView，因为那会调用removeFromParent）
        // 我们需要手动管理，因为卡牌a要移动到StackView
        auto fieldIt = std::find(playFieldView->_cardViews.begin(), playFieldView->_cardViews.end(), playFieldCardView);
        if (fieldIt != playFieldView->_cardViews.end()) {
            playFieldView->_cardViews.erase(fieldIt);
        }
        playFieldView->_cardIdMap.erase(playFieldCardView);
        
        // 从节点树移除
        playFieldCardView->removeFromParent();
        CCLOG("StackView::playPlayFieldCardMoveToStackAnimation: Card A removed from PlayFieldView");
    }
    
    // 2. 将卡牌a添加到StackView（作为最后一张卡牌）
    auto it = std::find(_cardViews.begin(), _cardViews.end(), playFieldCardView);
    if (it == _cardViews.end()) {
        // 添加到StackView（添加到列表末尾，成为最后一张）
        _cardViews.push_back(playFieldCardView);
        // 分配卡牌ID
        int cardId = _nextCardId++;
        _cardIdMap[playFieldCardView] = cardId;
        // 添加到节点树（现在parent是nullptr，可以安全添加）
        this->addChild(playFieldCardView);
        if (cardAParent) {
            playFieldCardView->release();  // 释放之前retain的引用
        }
        CCLOG("StackView::playPlayFieldCardMoveToStackAnimation: Added PlayField card to StackView with ID %d (as last card)", cardId);
    } else if (cardAParent) {
        playFieldCardView->release();  // 如果已经在列表中，也要释放retain
    }
    
    // 3. 移除卡牌b（b直接被a替换掉，不需要移动动画）
    if (oldTopCardView) {
        // 从StackView的列表中移除卡牌b
        auto bIt = std::find(_cardViews.begin(), _cardViews.end(), oldTopCardView);
        if (bIt != _cardViews.end()) {
            _cardViews.erase(bIt);
        }
        // 从映射中移除
        _cardIdMap.erase(oldTopCardView);
        // 从节点树移除
        oldTopCardView->removeFromParent();
        CCLOG("StackView::playPlayFieldCardMoveToStackAnimation: Card B removed (replaced by Card A)");
    }
    
    // 4. 更新Z轴顺序：卡牌a在最上层（作为最后一张卡牌）
    playFieldCardView->setLocalZOrder(_cardViews.size());
    
    // 5. 播放移动动画：卡牌a移动到Stack顶部位置（b的位置）
    float moveDuration = 0.3f;  // 动画时长
    auto moveAction = MoveTo::create(moveDuration, topPosition);
    
    // 动画完成后，更新卡牌位置
    StackView* self = this;
    auto callback = CallFunc::create([playFieldCardView, topPosition, self]() {
        playFieldCardView->setPosition(topPosition);
        CCLOG("StackView::playPlayFieldCardMoveToStackAnimation: Card A moved to stack top position (%.1f, %.1f)", 
              topPosition.x, topPosition.y);
        CCLOG("StackView::playPlayFieldCardMoveToStackAnimation: Card A is now the last card in Stack");
    });
    
    auto sequence = Sequence::create(moveAction, callback, nullptr);
    playFieldCardView->runAction(sequence);
}

void StackView::createUndoButton()
{
    // 创建回退按钮背景（使用LayerColor）
    Size buttonSize(120, 50);
    auto buttonBg = LayerColor::create(Color4B(100, 150, 200, 255), buttonSize.width, buttonSize.height);
    buttonBg->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    // 创建回退按钮文字标签
    _undoButtonLabel = Label::createWithSystemFont("回退", "Arial", 24);
    _undoButtonLabel->setColor(Color3B::WHITE);
    _undoButtonLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
    _undoButtonLabel->setPosition(Vec2(buttonSize.width / 2, buttonSize.height / 2));
    buttonBg->addChild(_undoButtonLabel);
    
    // 创建按钮节点容器
    _undoButton = Node::create();
    _undoButton->addChild(buttonBg);
    _undoButton->setContentSize(buttonSize);
    _undoButton->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    // 设置按钮位置（相对于StackView的右上角）
    // 注意：此时StackView的contentSize可能还未设置，位置会在setContentSize中更新
    Size stackSize = this->getContentSize();
    Vec2 buttonPos;
    if (stackSize.width > 0 && stackSize.height > 0) {
        // 如果StackView已经有大小，使用实际大小
        buttonPos = Vec2(stackSize.width - buttonSize.width / 2 - 20, stackSize.height - buttonSize.height / 2 - 20);
    } else {
        // 否则使用临时位置，稍后会在setContentSize中更新
        buttonPos = Vec2(buttonSize.width / 2 + 20, buttonSize.height / 2 + 20);
    }
    _undoButton->setPosition(buttonPos);
    
    // 添加触摸事件监听器
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        if (!_undoButton || !_undoButton->isVisible()) {
            return false;
        }
        
        // 将触摸位置转换为StackView的本地坐标
        Vec2 touchPos = this->convertToNodeSpace(touch->getLocation());
        
        // 计算按钮的本地坐标边界
        Vec2 buttonPos = _undoButton->getPosition();
        Size buttonSize = _undoButton->getContentSize();
        Rect buttonRect = Rect(
            buttonPos.x - buttonSize.width / 2,
            buttonPos.y - buttonSize.height / 2,
            buttonSize.width,
            buttonSize.height
        );
        
        if (buttonRect.containsPoint(touchPos)) {
            // 按钮被点击
            onUndoButtonClicked();
            return true;
        }
        
        return false;
    };
    
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, _undoButton);
    
    // 添加到StackView
    this->addChild(_undoButton, 1000);  // 高Z轴，确保在最上层
    
    // 初始状态：禁用（因为没有可回退的操作）
    updateUndoButtonState(false);
    
    CCLOG("StackView::createUndoButton: Undo button created at position (%.1f, %.1f)", buttonPos.x, buttonPos.y);
}

void StackView::setUndoButtonCallback(const std::function<void()>& callback)
{
    _undoButtonCallback = callback;
}

void StackView::updateUndoButtonState(bool canUndo)
{
    if (!_undoButton || !_undoButtonLabel) {
        return;
    }
    
    if (canUndo) {
        // 可以回退：按钮正常显示
        _undoButton->setVisible(true);
        _undoButtonLabel->setString("回退");
        _undoButtonLabel->setColor(Color3B::WHITE);
        
        // 更新背景颜色（正常状态）
        auto buttonBg = _undoButton->getChildren().at(0);
        if (buttonBg) {
            static_cast<LayerColor*>(buttonBg)->setColor(Color3B(100, 150, 200));
        }
    } else {
        // 不能回退：按钮变灰
        _undoButton->setVisible(true);
        _undoButtonLabel->setString("回退");
        _undoButtonLabel->setColor(Color3B(150, 150, 150));
        
        // 更新背景颜色（禁用状态）
        auto buttonBg = _undoButton->getChildren().at(0);
        if (buttonBg) {
            static_cast<LayerColor*>(buttonBg)->setColor(Color3B(80, 80, 80));
        }
    }
}

void StackView::onUndoButtonClicked()
{
    CCLOG("StackView::onUndoButtonClicked: Undo button clicked");
    
    if (_undoButtonCallback) {
        _undoButtonCallback();
    }
}
