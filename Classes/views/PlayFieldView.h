#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <map>
#include <functional>

USING_NS_CC;

class StackView;

// 游戏区域视图
class PlayFieldView : public Node {
    friend class StackView;  // 允许StackView访问私有成员，用于移动卡牌
public:
    // 创建 PlayFieldView
    static PlayFieldView* create();
    
    // 初始化 PlayFieldView
    virtual bool init() override;
    
    // 添加卡牌视图到游戏区域
    void addCardView(CardView* cardView);
    
    // 设置卡牌点击回调函数
    // callback - 回调函数，参数为被点击卡牌的ID
    void setCardClickCallback(const std::function<void(int cardId)>& callback);
    
    // 设置背景颜色
    void setBackgroundColor(const Color4B& color);
    
    // 设置背景颜色（重载版本）
    // r - 红色分量 (0-255)
    // g - 绿色分量 (0-255)
    // b - 蓝色分量 (0-255)
    // a - 透明度分量 (0-255)
    void setBackgroundColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
    
    // 设置 PlayFieldView 的大小（会同时更新背景层大小）
    void setContentSize(const Size& size) override;
    
    // 获取所有卡牌视图
    const std::vector<CardView*>& getCardViews() const { return _cardViews; }
    
    // 根据卡牌ID获取卡牌视图
    CardView* getCardViewById(int cardId) const;
    
    // 移除卡牌视图

    void removeCardView(CardView* cardView);
    
    // 添加卡牌视图到列表（不添加到节点树，用于回退时同步列表）
    void addCardViewToList(CardView* cardView);
    
private:
    // 触摸事件开始处理
    bool onTouchBegan(Touch* touch, Event* event);
    
    // 根据触摸位置找到最上层的卡牌视图
    CardView* findTopCard(const Vec2& worldPos);
    
    // 获取卡牌ID
    int getCardId(CardView* cardView) const;
    
private:
    std::vector<CardView*> _cardViews;  // 卡牌视图列表
    std::map<CardView*, int> _cardIdMap; // 卡牌视图到ID的映射
    int _nextCardId = 0;                 // 下一个卡牌ID
    
    std::function<void(int cardId)> _onCardClickCallback; // 卡牌点击回调函数
    
    LayerColor* _backgroundLayer = nullptr; // 背景层
};

