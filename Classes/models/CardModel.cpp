#include "CardModel.h"

CardModel::CardModel(CardSuitType suit, CardFaceType rank)
    : _suit(suit), _rank(rank) {
}

CardSuitType CardModel::getSuit() const {
    return _suit;
}

CardFaceType CardModel::getRank() const {
    return _rank;
}

bool CardModel::isFaceUp() const {
    return _faceUp;
}

void CardModel::setFaceUp(bool up) {
    _faceUp = up;
}
bool CardModel::isClickable() const {
    return _isClickable;
}
void CardModel::setClickable(bool c) {
    _isClickable = c;
}
Vec2 CardModel::getPosition() const { 
    return _position; 
}
void CardModel::setPosition(const Vec2& pos) {
    _position = pos; 
}

//通过从json文件获得的卡牌数据进行进一步的设置，将srack区的卡牌错位
std::vector<CardModel*> CardModel::createFromConfigs(const std::vector<CardResConfig>& configs, bool isStack) {
    
    std::vector<CardModel*> models;
    Vec2 stackBasePos(500, 200); // stack 区参考位置
    float offsetX = 50;           // stack 区错开间距

    for (size_t i = 0; i < configs.size(); ++i) {
        const auto& cfg = configs[i];
        CardModel* model = new CardModel((CardSuitType)cfg.suitImage, (CardFaceType)cfg.bigRankImage);

        if (isStack) {
            // 第一张放右边，其余向左错开
            Vec2 pos = stackBasePos;
            pos.x += (i == 0 ? 50 : -i * offsetX);
            model->setPosition(pos);
        }
        else {
            // playfield 用配置里的 playfieldPos
            model->setPosition(cfg.playfieldPos);
        }

        model->setFaceUp(false);
        model->setClickable(true);

        models.push_back(model);
    }
    return models;
}
//得到id
int CardModel::getId() const { return _id; }


//设置卡牌ID
void CardModel::setId(int id) { _id = id; }