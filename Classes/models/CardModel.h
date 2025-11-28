#pragma once
#include <string>
#include"../Classes/configs/models/CardResConfig.h"

// 卡牌上的花色
enum CardSuitType
{
    CST_NONE = -1,
    CST_CLUBS,      
    CST_DIAMONDS,   
    CST_HEARTS,     
    CST_SPADES,     
    CST_NUM_CARD_SUIT_TYPES
};
// 卡牌的数值
enum CardFaceType
{
    CFT_NONE = -1,
    CFT_ACE,
    CFT_TWO,
    CFT_THREE,
    CFT_FOUR,
    CFT_FIVE,
    CFT_SIX,
    CFT_SEVEN,
    CFT_EIGHT,
    CFT_NINE,
    CFT_TEN,
    CFT_JACK,
    CFT_QUEEN,
    CFT_KING,
    CFT_NUM_CARD_FACE_TYPES
};
//卡牌的数据
class CardModel {
public:
    CardModel(CardSuitType suit, CardFaceType rank);

    //得到花色
    CardSuitType getSuit() const;
    //得到数值
    CardFaceType getRank() const;

    //是否为正面
    bool isFaceUp() const;
    //设置正反面
    void setFaceUp(bool up);
    //是否可以点击
    bool isClickable() const;
    //设置点击
    void setClickable(bool c);

    //得到位置
    Vec2 getPosition() const;
    //设置位置
    void setPosition(const Vec2& pos);
    
    

     //返回: 卡牌的唯一标识ID
    int getId() const;
    
    
     //设置卡牌ID
    void setId(int id);
    
    //用来创建两堆卡牌，field区域和stack区域
    static std::vector<CardModel*> createFromConfigs(const std::vector<CardResConfig>& configs, bool isStack);

private:
    CardSuitType _suit;
    CardFaceType _rank;

    bool _faceUp = false;      //是否为正面（目前还没有使用）
    bool _isClickable = true;//是否可以点击（目前还未使用）

    Vec2 _position; // 位置
    
    int _id = -1;  // 卡牌唯一标识ID，-1表示未设置
};
