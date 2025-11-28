#include "CardView.h"
#include "../Classes/models/CardModel.h"

USING_NS_CC;

CardView* CardView::create(CardModel* model) {
    CardView* v = new CardView();
    if (v && v->init(model)) {
        v->autorelease();
        return v;
    }
    CC_SAFE_DELETE(v);
    return nullptr;
}

bool CardView::init(CardModel* model) {
    if (!model) {
        CCLOG("CardView::init: model is null");
        return false;
    }
    
    if (!Node::init()) {
        return false;
    }
    
    _model = model;
    // 保持锚点为中心点 (0.5, 0.5)，JSON文件中的位置是左下角
    // 我们会在设置位置时调整
    setAnchorPoint({ 0.5f, 0.5f });

    // 1. ��������
    std::string bgPath = "../Resources/suits/card_general.png";
    _bg = Sprite::create(bgPath);
    if (!_bg) {
        CCLOG("CardView::init: Failed to load card background: %s", bgPath.c_str());
        return false;
    }
    addChild(_bg);
    
    // 设置 CardView 的 ContentSize 为背景图片的大小
    setContentSize(_bg->getContentSize());

    // 2. 花色图标
    std::string suit_file = _suitPath();
    _suit = Sprite::create(suit_file);
    if (_suit) {
        _suit->setPosition(Vec2(50, 100));
        addChild(_suit);
    } else {
        CCLOG("CardView: Failed to load suit sprite: %s", suit_file.c_str());
    }

    // 3. 小点数图标
    std::string smallRankPath = _smallrankPath();
    _small_rank = Sprite::create(smallRankPath);
    if (_small_rank) {
        _small_rank->setPosition(Vec2(-50, 100));
        addChild(_small_rank);
    } else {
        CCLOG("CardView: Failed to load small rank sprite: %s", smallRankPath.c_str());
    }

    // 4. 大点数图标
    std::string bigRankPath = _bigrankPath();
    _big_rank = Sprite::create(bigRankPath);
    if (_big_rank) {
        _big_rank->setPosition(Vec2(0, -50));
        addChild(_big_rank);
    } else {
        CCLOG("CardView: Failed to load big rank sprite: %s", bigRankPath.c_str());
    }

    initTouchListener();



    return true;
}

std::string CardView::_suitPath() {
    switch (_model->getSuit()) {
    case CardSuitType::CST_CLUBS:    return "../Resources/suits/club.png";
    case CardSuitType::CST_DIAMONDS: return "../Resources/suits/diamond.png";
    case CardSuitType::CST_HEARTS:   return "../Resources/suits/heart.png";
    case CardSuitType::CST_SPADES:   return "../Resources/suits/spade.png";
    }
    return "";
}

std::string CardView::_smallrankPath()
{

    CardFaceType rank = _model->getRank();

    bool isRed = false;

    if (_model->getSuit() == CardSuitType::CST_DIAMONDS ||
        _model->getSuit() == CardSuitType::CST_HEARTS)
    {
        isRed = true;
    }
    else
    {
        isRed = false; 
    }
    if (isRed == true) {
        if (rank == CFT_ACE) return "../Resources/number/small_red_A.png";
        else if (rank == CFT_TWO) return "../Resources/number/small_red_2.png";
        else if (rank == CFT_THREE) return "../Resources/number/small_red_3.png";
        else if (rank == CFT_FOUR) return "../Resources/number/small_red_4.png";
        else if (rank == CFT_FIVE) return "../Resources/number/small_red_5.png";
        else if (rank == CFT_SIX) return "../Resources/number/small_red_6.png";
        else if (rank == CFT_SEVEN) return "../Resources/number/small_red_7.png";
        else if (rank == CFT_EIGHT) return "../Resources/number/small_red_8.png";
        else if (rank == CFT_NINE) return "../Resources/number/small_red_9.png";
        else if (rank == CFT_TEN) return "../Resources/number/small_red_10.png";
        else if (rank == CFT_JACK) return "../Resources/number/small_red_J.png";
        else if (rank == CFT_QUEEN) return "../Resources/number/small_red_Q.png";
        else if (rank == CFT_KING) return "../Resources/number/small_red_K.png";
    }
    else {
        if (rank == CFT_ACE) return "../Resources/number/small_black_A.png";
        else if (rank == CFT_TWO) return "../Resources/number/small_black_2.png";
        else if (rank == CFT_THREE) return "../Resources/number/small_black_3.png";
        else if (rank == CFT_FOUR) return "../Resources/number/small_black_4.png";
        else if (rank == CFT_FIVE) return "../Resources/number/small_black_5.png";
        else if (rank == CFT_SIX) return "../Resources/number/small_black_6.png";
        else if (rank == CFT_SEVEN) return "../Resources/number/small_black_7.png";
        else if (rank == CFT_EIGHT) return "../Resources/number/small_black_8.png";
        else if (rank == CFT_NINE) return "../Resources/number/small_black_9.png";
        else if (rank == CFT_TEN) return "../Resources/number/small_black_10.png";
        else if (rank == CFT_JACK) return "../Resources/number/small_black_J.png";
        else if (rank == CFT_QUEEN) return "../Resources/number/small_black_Q.png";
        else if (rank == CFT_KING) return "../Resources/number/small_black_K.png";
    }
    return "";
}
std::string CardView::_bigrankPath()
{

    CardFaceType rank = _model->getRank();

    bool isRed = false;

    if (_model->getSuit() == CardSuitType::CST_DIAMONDS ||
        _model->getSuit() == CardSuitType::CST_HEARTS)
    {
        isRed = true;
    }
    else
    {
        isRed = false; 
    }

    if (isRed == true) {
        if (rank == CFT_ACE) return "../Resources/number/big_red_A.png";
        else if (rank == CFT_TWO) return "../Resources/number/big_red_2.png";
        else if (rank == CFT_THREE) return "../Resources/number/big_red_3.png";
        else if (rank == CFT_FOUR) return "../Resources/number/big_red_4.png";
        else if (rank == CFT_FIVE) return "../Resources/number/big_red_5.png";
        else if (rank == CFT_SIX) return "../Resources/number/big_red_6.png";
        else if (rank == CFT_SEVEN) return "../Resources/number/big_red_7.png";
        else if (rank == CFT_EIGHT) return "../Resources/number/big_red_8.png";
        else if (rank == CFT_NINE) return "../Resources/number/big_red_9.png";
        else if (rank == CFT_TEN) return "../Resources/number/big_red_10.png";
        else if (rank == CFT_JACK) return "../Resources/number/big_red_J.png";
        else if (rank == CFT_QUEEN) return "../Resources/number/big_red_Q.png";
        else if (rank == CFT_KING) return "../Resources/number/big_red_K.png";
    }
    else {
        if (rank == CFT_ACE) return "../Resources/number/big_black_A.png";
        else if (rank == CFT_TWO) return "../Resources/number/big_black_2.png";
        else if (rank == CFT_THREE) return "../Resources/number/big_black_3.png";
        else if (rank == CFT_FOUR) return "../Resources/number/big_black_4.png";
        else if (rank == CFT_FIVE) return "../Resources/number/big_black_5.png";
        else if (rank == CFT_SIX) return "../Resources/number/big_black_6.png";
        else if (rank == CFT_SEVEN) return "../Resources/number/big_black_7.png";
        else if (rank == CFT_EIGHT) return "../Resources/number/big_black_8.png";
        else if (rank == CFT_NINE) return "../Resources/number/big_black_9.png";
        else if (rank == CFT_TEN) return "../Resources/number/big_black_10.png";
        else if (rank == CFT_JACK) return "../Resources/number/big_black_J.png";
        else if (rank == CFT_QUEEN) return "../Resources/number/big_black_Q.png";
        else if (rank == CFT_KING) return "../Resources/number/big_black_K.png";
    }
    return "";
}

void CardView::refresh()
{

    _suit->setTexture(_suitPath());
    _small_rank->setTexture(_smallrankPath());
    _big_rank->setTexture(_bigrankPath());

    setPosition(_model->getPosition());
}

void CardView::setSelected(bool v)
{

    setScale(v ? 1.1f : 1.0f);
}
void CardView::setClickCallback(const std::function<void(CardView*)>& callback)
{
    _clickCallback = callback;
}

void CardView::initTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        Vec2 location = this->convertToNodeSpace(touch->getLocation());
        if (getBoundingBox().containsPoint(location)) {
            if (_clickCallback) _clickCallback(this);
            return true;
        }
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}