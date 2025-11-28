#pragma once
#include "cocos2d.h"

USING_NS_CC;

// 菜单场景
// 职责：显示关卡选择菜单，点击按钮后启动对应关卡的游戏场景
class MenuScene : public Scene {
public:
    static Scene* createScene();
    
    virtual bool init() override;
    
    // 使用 Cocos2d-x 标准宏创建场景实例
    CREATE_FUNC(MenuScene);
    
private:
    // 关卡按钮点击处理
    // levelNumber - 关卡编号（1, 2, 3...）
    void onLevelButtonClicked(int levelNumber);
    
    // 关卡按钮点击回调（用于 MenuItemLabel）
    void onLevelButtonCallback(Ref* sender);
    
    // 创建关卡选择按钮（使用 MenuItemLabel）
    // levelNumber - 关卡编号
    // position - 按钮位置（世界坐标）
    // 返回：创建的菜单项，失败返回 nullptr
    MenuItemLabel* createLevelButton(int levelNumber, const Vec2& position);
};

