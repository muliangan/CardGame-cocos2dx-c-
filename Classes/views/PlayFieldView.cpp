#include "PlayFieldView.h"
#include "CardView.h"
#include "cocos2d.h"
#include <algorithm>

USING_NS_CC;

PlayFieldView* PlayFieldView::create()
{
    PlayFieldView* ret = new PlayFieldView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool PlayFieldView::init()
{
    if (!Node::init()) {
        return false;
    }
    
    // 设置锚点为左下角 (0, 0)，确保坐标系统一致
    setAnchorPoint(Vec2::ZERO);
    
    // 创建背景层（默认浅蓝色）
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _backgroundLayer = LayerColor::create(Color4B(200, 200, 255, 255), visibleSize.width, visibleSize.height);
    if (_backgroundLayer) {
        _backgroundLayer->setAnchorPoint(Vec2::ZERO);
        _backgroundLayer->setPosition(Vec2::ZERO);
        this->addChild(_backgroundLayer, -1); // 背景层在最底层
    }
    
    // 注册触摸事件监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(PlayFieldView::onTouchBegan, this);
    listener->setSwallowTouches(true); // 吞噬触摸事件，确保PlayFieldView优先处理
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    return true;
}

void PlayFieldView::addCardView(CardView* cardView)
{
    if (!cardView) {
        return;
    }
    
    // 分配卡牌ID
    int cardId = _nextCardId++;
    _cardIdMap[cardView] = cardId;
    _cardViews.push_back(cardView);
    
    // 将卡牌视图添加到节点树
    this->addChild(cardView);
}

void PlayFieldView::setCardClickCallback(const std::function<void(int cardId)>& callback)
{
    _onCardClickCallback = callback;
}

void PlayFieldView::setBackgroundColor(const Color4B& color)
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

void PlayFieldView::setBackgroundColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
    setBackgroundColor(Color4B(r, g, b, a));
}

void PlayFieldView::setContentSize(const Size& size)
{
    Node::setContentSize(size);
    
    // 更新背景层大小
    if (_backgroundLayer) {
        _backgroundLayer->changeWidthAndHeight(size.width, size.height);
    } else {
        // 如果背景层不存在，创建一个新的
        _backgroundLayer = LayerColor::create(Color4B(200, 200, 255, 255), size.width, size.height);
        _backgroundLayer->setAnchorPoint(Vec2::ZERO);
        _backgroundLayer->setPosition(Vec2::ZERO);
        this->addChild(_backgroundLayer, -1);
    }
}

bool PlayFieldView::onTouchBegan(Touch* touch, Event* event)
{
    if (!touch) {
        return false;
    }
    
    // 获取触摸位置（世界坐标）
    Vec2 worldPos = touch->getLocation();
    worldPos.y += 290.0f;
    // 查找最上层的卡牌（使用世界坐标）
    CardView* topCard = findTopCard(worldPos);
    
    if (topCard) {
        // 获取卡牌ID
        int cardId = getCardId(topCard);
        
        if (cardId >= 0 && _onCardClickCallback) {
            // 调用回调函数
            _onCardClickCallback(cardId);
            return true;
        }
    }
    
    return false;
}

CardView* PlayFieldView::findTopCard(const Vec2& worldPos)
{
    // 从后往前遍历（最后添加的在最上层）
    // 这样可以确保如果多张卡牌重叠，点击的是最上层的那张
    int cardIndex = 0;
    for (auto it = _cardViews.rbegin(); it != _cardViews.rend(); ++it, ++cardIndex) {
        CardView* cardView = *it;
        if (cardView && cardView->isVisible()) {
            // 获取CardView的世界坐标位置（中心点）
            Vec2 cardWorldPos = this->convertToWorldSpace(cardView->getPosition());
            Size cardSize = cardView->getContentSize();  // 卡牌大小
            
            // CardView锚点是中心点(0.5, 0.5)，在世界坐标系中计算边界框：
            float worldLeft = cardWorldPos.x - cardSize.width;   // 左上角X
            float worldTop = cardWorldPos.y + cardSize.height;   // 左上角Y
            float worldRight = cardWorldPos.x ;  // 右下角X
            float worldBottom = cardWorldPos.y; // 右下角Y
            
            // 检查触摸点是否在卡牌的边界框内（使用世界坐标）
            if (worldPos.x >= worldLeft && worldPos.x <= worldRight &&
                worldPos.y >= worldBottom && worldPos.y <= worldTop) {
                return cardView;
            }
        }
    }
    
    return nullptr;
}

int PlayFieldView::getCardId(CardView* cardView) const
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

CardView* PlayFieldView::getCardViewById(int cardId) const
{
    for (const auto& pair : _cardIdMap) {
        if (pair.second == cardId) {
            return pair.first;
        }
    }
    return nullptr;
}

void PlayFieldView::removeCardView(CardView* cardView)
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
}

void PlayFieldView::addCardViewToList(CardView* cardView)
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
    }
}
