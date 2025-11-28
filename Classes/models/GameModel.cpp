#include "GameModel.h"
#include <algorithm>
// 初始化 playfield 和 stack 的卡牌
void GameModel::init(const std::vector<CardModel*>& playfieldCards,
    const std::vector<CardModel*>& stackCards)
{
    _playfieldCards = playfieldCards;
    _stackCards = stackCards;
}

const std::vector<CardModel*>& GameModel::getPlayfieldCards() const { return _playfieldCards; }

const std::vector<CardModel*>& GameModel::getStackCards() const { return _stackCards; }
//返回stack顶部卡牌
CardModel* GameModel::getTopStackCard() const
{
    if (_stackCards.empty()) return nullptr;
    return _stackCards.back();
}
//返回倒数第二张卡牌(顶部下一张卡牌)
CardModel* GameModel::getSecondTopStackCard() const
{
    if (_stackCards.size() < 2) return nullptr;
    return _stackCards[_stackCards.size() - 2];
}

void GameModel::replaceTopStackCard(CardModel* newCard)
{
    if (_stackCards.empty()) return;

    // 移除旧的堆顶卡牌
    _stackCards.pop_back();

    // 新卡牌入堆
    _stackCards.push_back(newCard);
}

// 将PlayField的卡牌移到Stack顶部，并从PlayField移除
// 返回被替换的顶部牌
CardModel* GameModel::movePlayFieldCardToStackTop(CardModel* playFieldCard)
{
    if (!playFieldCard || _stackCards.empty()) {
        return nullptr;
    }
    
    // 检查卡牌是否在PlayField中
    auto fieldIt = std::find(_playfieldCards.begin(), _playfieldCards.end(), playFieldCard);
    if (fieldIt == _playfieldCards.end()) {
        return nullptr;  // 卡牌不在PlayField中
    }
    
    // 获取被替换的顶部牌
    CardModel* oldTopCard = _stackCards.back();
    
    // 从PlayField移除
    _playfieldCards.erase(fieldIt);
    
    // 替换Stack顶部牌
    _stackCards.back() = playFieldCard;
    
    return oldTopCard;
}

CardModel* GameModel::moveCardToTop(CardModel* card)
{
    if (!card || _stackCards.empty()) {
        return nullptr;
    }
    
    // 如果已经是顶部牌，不需要移动
    if (_stackCards.back() == card) {
        return nullptr;
    }
    
    // 找到卡牌在Stack中的位置
    auto clickedIt = std::find(_stackCards.begin(), _stackCards.end(), card);
    if (clickedIt == _stackCards.end()) {
        return nullptr;  // 卡牌不在Stack中
    }
    
    // 获取顶部牌（被替换的牌）
    CardModel* oldTopCard = _stackCards.back();
    
    // 移除原来的顶部牌
    _stackCards.pop_back();
    
    // 从原位置移除点击的卡牌
    _stackCards.erase(clickedIt);
    
    // 将点击的卡牌添加到顶部
    _stackCards.push_back(card);
    
    return oldTopCard;
}

bool GameModel::canMatch(CardModel* playfieldCard) const
{
    CardModel* top = getTopStackCard();
    if (!top || !playfieldCard) return false;

    // 简单规则：点数差为1即可匹配
    int topRank = static_cast<int>(top->getRank());
    int cardRank = static_cast<int>(playfieldCard->getRank());

    return std::abs(topRank - cardRank) == 1;
}

bool GameModel::isInPlayfield(CardModel* card) const
{
    if (!card) return false;
    for (auto* c : _playfieldCards) {
        if (c == card) {
            return true;
        }
    }
    return false;
}

bool GameModel::isInStack(CardModel* card) const
{
    if (!card) return false;
    for (auto* c : _stackCards) {
        if (c == card) {
            return true;
        }
    }
    return false;
}
