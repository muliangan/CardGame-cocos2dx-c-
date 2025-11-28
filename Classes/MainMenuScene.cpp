#include "MainMenuScene.h"
#include "scene/CardTestScene.h"
cocos2d::Vec2 buttonPos;
USING_NS_CC;
#if 0
Scene* MainMenuScene::createScene()
{
    auto scene= MainMenuScene::create();
    return scene;
}

bool MainMenuScene::init()
{
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建标题
    auto titleLabel = Label::createWithTTF("Card Game", "fonts/Marker Felt.ttf", 48);
    if (titleLabel) {
        titleLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
            origin.y + visibleSize.height - 100));
        this->addChild(titleLabel, 1);
    }

    // 创建第一关按钮（屏幕中央）
    // 屏幕中心坐标：x = 1080/2 = 540, y = 2080/2 = 1040
    buttonPos.x = visibleSize.width / 2;
    buttonPos.y = visibleSize.height / 2;

    auto levelButton = createLevelButton(1, buttonPos);

    if (levelButton) {
        // 创建菜单
        auto menu = Menu::create(levelButton, nullptr);
        menu->setPosition(Vec2::ZERO);
        menu->setAnchorPoint(Vec2(0, 0));  // 菜单锚点在左下角 (0,0)
        this->addChild(menu, 1);
    }

    return true;
}

MenuItemLabel* MainMenuScene::createLevelButton(int levelNumber, const Vec2& position)
{
    // 创建按钮文字
    std::string buttonText = "Level " + std::to_string(levelNumber);
    Label* label = Label::createWithTTF(buttonText, "fonts/Marker Felt.ttf", 32);

    if (!label) {
        return nullptr;
    }

    // 设置 Label 的锚点为中心
    label->setAnchorPoint(Vec2(0.5f, 0.5f));

    // 创建菜单项，使用成员函数作为回调
    MenuItemLabel* menuItem = MenuItemLabel::create(label, this, menu_selector(MainMenuScene::onLevelButtonCallback));

    if (menuItem) {
        menuItem->setPosition(position);
        menuItem->setAnchorPoint(Vec2(0.5f, 0.5f));  // 设置锚点为中心，确保居中
        menuItem->setTag(levelNumber);  // 使用 tag 存储关卡编号
        menuItem->setColor(Color3B::YELLOW);
        menuItem->setScale(2.5f);
    }

    return menuItem;
}

void MainMenuScene::onLevelButtonCallback(Ref* sender)
{
    MenuItemLabel* menuItem = dynamic_cast<MenuItemLabel*>(sender);
    if (menuItem) {
        int levelNumber = menuItem->getTag();
        onLevelButtonClicked(levelNumber);
    }
}

void MainMenuScene::onLevelButtonClicked(int levelNumber)
{
    CCLOG("User selected level %d", levelNumber);

    // 切换到测试场景，传入关卡编号
    auto testScene = CardTestScene::createScene(levelNumber);
    if (testScene) {
        Director::getInstance()->replaceScene(testScene);
    }
}
#endif