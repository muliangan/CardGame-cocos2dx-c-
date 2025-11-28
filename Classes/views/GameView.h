#pragma once
#include "cocos2d.h"
#include "CardView.h"
#include "../Classes/models/GameModel.h"
#include "PlayFieldView.h"
#include "StackView.h"
#include "../managers/UndoManager.h"
USING_NS_CC;

class GameView : public Node {
public:
    // 创建 GameView
    static GameView* create(GameModel* model);

    // 初始化 GameView
    bool init(GameModel* model);
    
    // 初始化 UI（设置初始布局，创建 CardView）
    void initUI(PlayFieldView* playFieldView, StackView* stackView);

    // 添加一个卡牌到对应视图
    // isStack = true 表示堆叠区域，false 表示游戏区域
    void addCardView(CardView* cardView, bool isStack);

    // 设置 Card 点击回调（Controller 调用）
    void setCardClickCallback(const std::function<void(CardView*)>& callback);
    

    //所有卡牌依次出现，带有淡入和缩放效果
    void playEntranceAnimation();
    

    //收集所有卡牌的位置信息（用于回退功能）
    //返回: 卡牌ID到位置信息的映射
    std::map<int, struct CardPositionInfo> collectCardPositions() const;
    
    // 根据CardModel ID查找对应的CardView
    // cardId - 卡牌ID
    // 返回: CardView指针，如果不存在返回nullptr
    CardView* findCardViewByModelId(int cardId) const;
    
    // 播放回退动画：将卡牌移动到指定位置
    // cardView - 要移动的卡牌视图
    // targetWorldPos - 目标世界坐标位置
    // isInStack - 是否在Stack区域
    void playUndoAnimation(CardView* cardView, const Vec2& targetWorldPos, bool isInStack);

    // 获取PlayFieldView和StackView（供GameController使用）
    PlayFieldView* getPlayFieldView() const { return _playFieldView; }
    StackView* getStackView() const { return _stackView; }
    
private:
    GameModel* _gameModel = nullptr;  // 游戏逻辑模块（管理状态，不管理 UI）

    PlayFieldView* _playFieldView = nullptr;  // field区域视图
    StackView* _stackView = nullptr;          // stack区域视图

    std::vector<CardView*> _fieldCardViews; // field卡牌视图
    std::vector<CardView*> _stackCardViews; // stack卡牌视图

    std::function<void(CardView*)> _onCardClickCallback; // Card 点击回调
    
    // 允许GameController访问私有成员
    friend class GameController;

    // Touch 事件处理，只负责转发给 Controller 处理逻辑
    bool onTouchBegan(Touch* touch, Event* event);

    // 根据点击位置找到最上层的 CardView
    CardView* findTopCard(const Vec2& pos);
};
