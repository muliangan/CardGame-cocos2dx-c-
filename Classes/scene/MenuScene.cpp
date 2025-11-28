#include "MenuScene.h"
#include "GameScene.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    auto scene = MenuScene::create();
    return scene;
}

bool MenuScene::init()
{
    if (!Scene::init()) {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // 设置场景背景色
    auto layerColor = LayerColor::create(Color4B(50, 50, 100, 255), visibleSize.width, visibleSize.height);
    if (layerColor) {
        layerColor->setPosition(origin);
        layerColor->setAnchorPoint(Vec2(0, 0));
        this->addChild(layerColor, 0);
    }
    
    // 创建标题
    auto titleLabel = Label::createWithTTF("选择关卡", "fonts/Marker Felt.ttf", 48);
    if (!titleLabel) {
        titleLabel = Label::createWithSystemFont("选择关卡", "Arial", 48);
    }
    
    if (titleLabel) {
        titleLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
                                    origin.y + visibleSize.height - 100));
        titleLabel->setColor(Color3B::WHITE);
        titleLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
        this->addChild(titleLabel, 1);
    }
    
    // 创建多个关卡按钮（示例：创建3个关卡按钮）
    Vector<MenuItem*> menuItems;
    
    // 关卡1按钮
    Vec2 buttonPos1(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 + 100);
    auto levelButton1 = createLevelButton(1, buttonPos1);
    if (levelButton1) {
        menuItems.pushBack(levelButton1);
    }
    
    // 关卡2按钮
    Vec2 buttonPos2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
    auto levelButton2 = createLevelButton(2, buttonPos2);
    if (levelButton2) {
        menuItems.pushBack(levelButton2);
    }
    
    // 关卡3按钮
    Vec2 buttonPos3(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 100);
    auto levelButton3 = createLevelButton(3, buttonPos3);
    if (levelButton3) {
        menuItems.pushBack(levelButton3);
    }
    
    // 创建菜单
    if (menuItems.size() > 0) {
        auto menu = Menu::createWithArray(menuItems);
        menu->setPosition(Vec2::ZERO);
        menu->setAnchorPoint(Vec2(0, 0));
        this->addChild(menu, 1);
    }
    
    return true;
}

MenuItemLabel* MenuScene::createLevelButton(int levelNumber, const Vec2& position)
{
    // 创建按钮文字
    std::string buttonText = "Level " + std::to_string(levelNumber);
    Label* label = Label::createWithTTF(buttonText, "fonts/Marker Felt.ttf", 32);
    
    if (!label) {
        label = Label::createWithSystemFont(buttonText, "Arial", 32);
    }
    
    if (!label) {
        return nullptr;
    }
    
    // 设置 Label 的锚点为中心
    label->setAnchorPoint(Vec2(0.5f, 0.5f));
    
    // 创建菜单项，使用成员函数作为回调
    MenuItemLabel* menuItem = MenuItemLabel::create(label, this, menu_selector(MenuScene::onLevelButtonCallback));
    
    if (menuItem) {
        menuItem->setPosition(position);
        menuItem->setAnchorPoint(Vec2(0.5f, 0.5f));
        menuItem->setTag(levelNumber);  // 使用 tag 存储关卡编号
        menuItem->setColor(Color3B::YELLOW);
        menuItem->setScale(2.0f);
    }
    
    return menuItem;
}

void MenuScene::onLevelButtonCallback(Ref* sender)
{
    MenuItemLabel* menuItem = dynamic_cast<MenuItemLabel*>(sender);
    if (menuItem) {
        int levelNumber = menuItem->getTag();
        onLevelButtonClicked(levelNumber);
    }
}

void MenuScene::onLevelButtonClicked(int levelNumber)
{
    CCLOG("MenuScene: User selected level %d", levelNumber);
    
    // 切换到游戏场景，传入关卡编号
    auto gameScene = GameScene::createScene(levelNumber);
    if (gameScene) {
        Director::getInstance()->replaceScene(gameScene);
    } else {
        CCLOG("MenuScene: Failed to create GameScene for level %d", levelNumber);
    }
}

