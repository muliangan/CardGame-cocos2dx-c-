#pragma once
#include "cocos2d.h"

USING_NS_CC;

// 菜单场景
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

    MenuItemLabel* createLevelButton(int levelNumber, const Vec2& position);
}; #pragma once
