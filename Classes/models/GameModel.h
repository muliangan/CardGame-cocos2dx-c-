#pragma once
#include "cocos2d.h"
#include "CardModel.h"
#include <vector>

class GameModel {
public:
    GameModel() = default;

    // 初始化 playfield 和 stack 的卡牌
    void init(const std::vector<CardModel*>& playfieldCards,const std::vector<CardModel*>& stackCards);

    // 获取卡牌
    const std::vector<CardModel*>& getPlayfieldCards() const;
    const std::vector<CardModel*>& getStackCards() const;

    // 获取堆顶卡牌
    CardModel* getTopStackCard() const;
    
    // 获取倒数第二张卡牌（用来替换顶部牌的卡牌）
    CardModel* getSecondTopStackCard() const;

    // 更新卡牌位置，只做数据更新
    void replaceTopStackCard(CardModel* newCard);
    
    // 将PlayField的卡牌移到Stack顶部，并从PlayField移除
    // 返回被替换的顶部牌
    CardModel* movePlayFieldCardToStackTop(CardModel* playFieldCard);
    
    // 将Stack中的某张卡牌移动到顶部（替换顶部牌）
    // 返回被替换的顶部牌，如果卡牌已经是顶部牌或操作失败，返回nullptr
    CardModel* moveCardToTop(CardModel* card);

    // 匹配逻辑，根据规则判断某个 playfield 卡是否可以放到 stack
    bool canMatch(CardModel* playfieldCard) const;

    // 判断卡牌是否在游戏区域
    bool isInPlayfield(CardModel* card) const;

    // 判断卡牌是否在堆叠区域
    bool isInStack(CardModel* card) const;

private:
    std::vector<CardModel*> _playfieldCards;  // 游戏卡牌（field）
    std::vector<CardModel*> _stackCards;      // 卡牌堆（stack）
};
