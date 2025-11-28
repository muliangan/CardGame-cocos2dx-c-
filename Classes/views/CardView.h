#pragma once
#include<cocos2d.h>
#include "../Classes/models/CardModel.h"

USING_NS_CC;

class CardView : public Node {
public:
    static CardView* create(CardModel* model);

    bool init(CardModel* model);

    void setSelected(bool v);     // 可选（做高亮）
    void refresh();               // Model 变化时 UI更新

    CardModel* getModel() const { return _model; }

    // 设置点击回调（由 Controller 设置）
    void setClickCallback(const std::function<void(CardView*)>& callback);
private:
    CardModel* _model = nullptr;    // 牌面数据

    Sprite* _bg = nullptr;    // 背景图
    Sprite* _suit = nullptr;  // 花色
    Sprite* _small_rank = nullptr;  // 数字
    Sprite* _big_rank = nullptr;

    void initTouchListener();
    std::function<void(CardView*)> _clickCallback;  // 点击回调
    
    std::string _suitPath();
    std::string _smallrankPath();
    std::string _bigrankPath();
};
