#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include <vector>
#include <map>
#include <functional>

USING_NS_CC;

// 堆叠区域视图
// 管理堆叠区域的卡牌显示，处理触摸事件，检测卡牌点击
class StackView : public Node {
public:
    // 创建 StackView
    static StackView* create();
    
    // 初始化 StackView
    virtual bool init() override;
    
    // 添加卡牌视图到堆叠区域
    // cardView - 要添加的卡牌视图
    void addCardView(CardView* cardView);
    
    // 设置卡牌点击回调函数
    // callback - 回调函数，参数为被点击卡牌的ID
    void setCardClickCallback(const std::function<void(int cardId)>& callback);
    
    // 设置背景颜色
    // color - 背景颜色（RGBA）
    void setBackgroundColor(const Color4B& color);
    
    // 设置背景颜色（重载版本）
    // r - 红色分量 (0-255)
    // g - 绿色分量 (0-255)
    // b - 蓝色分量 (0-255)
    // a - 透明度分量 (0-255)
    void setBackgroundColor(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
    
    // 设置 StackView 的大小
    void setContentSize(const Size& size) override;
    
    // 获取所有卡牌视图
    const std::vector<CardView*>& getCardViews() const { return _cardViews; }
    
    // 根据卡牌ID获取卡牌视图
    // cardId - 卡牌ID
    CardView* getCardViewById(int cardId) const;
    
    // 移除卡牌视图
    // cardView - 要移除的卡牌视图
    void removeCardView(CardView* cardView);
    
    // 添加卡牌视图到列表（不添加到节点树，用于回退时同步列表）
    void addCardViewToList(CardView* cardView);
    
    // 播放卡牌移动到顶部位置的动画（点击的牌a移动到顶部，原来的顶部牌b直接消失）
    // clickedCardView - 被点击的卡牌视图（移动到顶部）
    // oldTopCardView - 原来的顶部卡牌视图（将被移除）
    // topPosition - 顶部位置（Stack区最后一张牌的位置）
    void playCardMoveToTopAnimation(CardView* clickedCardView, CardView* oldTopCardView, 
                                     const Vec2& topPosition);
    
    // 获取顶部卡牌的位置（Stack区最后一张牌的位置）
    Vec2 getTopCardPosition() const;
    
    // 播放PlayField卡牌移动到Stack顶部的动画
    // playFieldCardView - 从PlayField移动过来的卡牌视图
    // oldTopCardView - 原来的顶部卡牌视图（将被移除）
    // topPosition - Stack顶部位置
    // playFieldView - PlayFieldView指针，用于在动画完成后移除卡牌
    void playPlayFieldCardMoveToStackAnimation(CardView* playFieldCardView, CardView* oldTopCardView, 
                                               const Vec2& topPosition, class PlayFieldView* playFieldView);
    
    // 设置回退按钮点击回调
    // callback - 回调函数，点击回退按钮时调用
    void setUndoButtonCallback(const std::function<void()>& callback);
    
    // 更新回退按钮状态（是否可回退）

    void updateUndoButtonState(bool canUndo);
    
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
    
    // 回退按钮相关
    Node* _undoButton = nullptr;  // 回退按钮节点
    Label* _undoButtonLabel = nullptr;  // 回退按钮文字标签
    std::function<void()> _undoButtonCallback;  // 回退按钮点击回调
    
    // 创建回退按钮
    void createUndoButton();
    
    // 回退按钮点击处理
    void onUndoButtonClicked();
};

