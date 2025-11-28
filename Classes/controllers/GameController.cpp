#include "GameController.h"
#include "../configs/loaders/LevelConfigLoader.h"
#include "../services/GameModelFromLevelGenerator.h"
#include "../views/PlayFieldView.h"
#include "../views/StackView.h"
#include "../views/CardView.h"
#include "cocos2d.h"
#include <set>

USING_NS_CC;

GameController::GameController()
{
    _undoManager = new UndoManager();
    _playFieldController = new PlayFieldController();
    _stackController = new StackController();
}

GameController::~GameController()
{
    CC_SAFE_DELETE(_gameModel);
    CC_SAFE_DELETE(_gameView);
    CC_SAFE_DELETE(_undoManager);
    CC_SAFE_DELETE(_playFieldController);
    CC_SAFE_DELETE(_stackController);
}

bool GameController::startGame(int levelId)
{
    _currentLevelId = levelId;
    
    CCLOG("GameController::startGame: Starting level %d", levelId);
    
    // 1. 加载关卡配置
    LevelConfig levelConfig = LevelConfigLoader::loadLevelConfig(levelId);
    if (levelConfig.playfieldCards.empty() && levelConfig.stackCards.empty()) {
        CCLOG("GameController::startGame: Failed to load level config for level %d", levelId);
        return false;
    }
    
    CCLOG("GameController::startGame: Loaded level config - PlayField: %d cards, Stack: %d cards",
        static_cast<int>(levelConfig.playfieldCards.size()),
        static_cast<int>(levelConfig.stackCards.size()));
    
    // 2. 生成 GameModel
    _gameModel = GameModelFromLevelGenerator::generateGameModel(levelConfig);
    if (!_gameModel) {
        CCLOG("GameController::startGame: Failed to generate game model");
        return false;
    }
    
    CCLOG("GameController::startGame: GameModel generated successfully");
    
    // 3. 初始化子控制器
    initSubControllers();
    
    // 4. 初始化游戏视图（如果已经设置）
    if (_gameView) {
        initGameView();
    }
    
    CCLOG("GameController::startGame: Game started successfully");
    return true;
}

void GameController::setGameView(GameView* gameView)
{
    _gameView = gameView;
    
    // 如果 GameModel 已经创建，初始化视图
    if (_gameModel && _gameView) {
        initGameView();
    }
}

void GameController::initSubControllers()
{
    if (!_gameModel) {
        CCLOG("GameController::initSubControllers: GameModel is null");
        return;
    }
    
    // 初始化 UndoManager
    _undoManager->init(_gameModel);
    CCLOG("GameController::initSubControllers: UndoManager initialized");
    
    // 初始化 PlayFieldController
    if (_playFieldController) {
        _playFieldController->init(_gameModel, _undoManager, this);
        CCLOG("GameController::initSubControllers: PlayFieldController initialized");
    }
    
    // 初始化 StackController
    if (_stackController) {
        _stackController->init(_gameModel, _undoManager, this);
        CCLOG("GameController::initSubControllers: StackController initialized");
    }
}

void GameController::initGameView()
{
    if (!_gameView || !_gameModel) {
        CCLOG("GameController::initGameView: GameView or GameModel is null");
        return;
    }
    
    // 创建 PlayFieldView 和 StackView
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    float playfieldHeight = 1500.0f;
    float stackHeight = visibleSize.height - playfieldHeight;
    
    // 创建 PlayFieldView
    PlayFieldView* playFieldView = PlayFieldView::create();
    playFieldView->setPosition(Vec2(origin.x, origin.y + stackHeight));
    playFieldView->setContentSize(Size(visibleSize.width, playfieldHeight));
    playFieldView->setBackgroundColor(200, 200, 255, 255);
    
    // 创建 StackView
    StackView* stackView = StackView::create();
    stackView->setPosition(Vec2(origin.x, origin.y));
    stackView->setContentSize(Size(visibleSize.width, stackHeight));
    stackView->setBackgroundColor(200, 255, 200, 255);
    
    // 初始化子控制器的视图
    if (_playFieldController) {
        _playFieldController->initView(playFieldView);
        // 设置StackView引用，用于播放动画
        _playFieldController->setStackView(stackView);
    }
    if (_stackController) {
        _stackController->initView(stackView);
    }
    
    // 初始化 GameView UI
    _gameView->initUI(playFieldView, stackView);
    
    // 设置回退按钮回调
    if (stackView) {
        stackView->setUndoButtonCallback([this, stackView]() {
            this->undo();
            // 更新按钮状态
            if (stackView) {
                stackView->updateUndoButtonState(this->canUndo());
            }
        });
        // 初始按钮状态
        stackView->updateUndoButtonState(this->canUndo());
    }
    
    CCLOG("GameController::initGameView: GameView initialized successfully");
}
bool GameController::undo()
{
    if (!_undoManager || !_gameView || !_gameModel) {
        return false;
    }
    
    // 获取撤销快照
    UndoSnapshot* snapshot = _undoManager->undo();
    if (!snapshot) {
        CCLOG("GameController::undo: Cannot undo");
        return false;
    }
    
    CCLOG("GameController::undo: Undo successful, restoring view state");
    
    // 恢复Model后，需要更新View以匹配Model状态
    
    // 1. 收集当前View中所有CardView的ID
    std::set<int> currentViewCardIds;
    PlayFieldView* playFieldView = _gameView->getPlayFieldView();
    StackView* stackView = _gameView->getStackView();
    
    if (playFieldView) {
        const auto& fieldViews = playFieldView->getCardViews();
        for (auto* cv : fieldViews) {
            if (cv && cv->getModel()) {
                currentViewCardIds.insert(cv->getModel()->getId());
            }
        }
    }
    if (stackView) {
        const auto& stackViews = stackView->getCardViews();
        for (auto* cv : stackViews) {
            if (cv && cv->getModel()) {
                currentViewCardIds.insert(cv->getModel()->getId());
            }
        }
    }
    
    // 2. 收集Model中所有卡牌的ID
    std::set<int> modelCardIds;
    for (auto* cardModel : _gameModel->getPlayfieldCards()) {
        if (cardModel) {
            modelCardIds.insert(cardModel->getId());
        }
    }
    for (auto* cardModel : _gameModel->getStackCards()) {
        if (cardModel) {
            modelCardIds.insert(cardModel->getId());
        }
    }
    
    // 3. 找出需要移除的CardView（View中有但Model中没有）
    std::vector<CardView*> cardsToRemove;
    if (playFieldView) {
        const auto& fieldViews = playFieldView->getCardViews();
        for (auto* cv : fieldViews) {
            if (cv && cv->getModel()) {
                int cardId = cv->getModel()->getId();
                if (modelCardIds.find(cardId) == modelCardIds.end()) {
                    cardsToRemove.push_back(cv);
                }
            }
        }
    }
    if (stackView) {
        const auto& stackViews = stackView->getCardViews();
        for (auto* cv : stackViews) {
            if (cv && cv->getModel()) {
                int cardId = cv->getModel()->getId();
                if (modelCardIds.find(cardId) == modelCardIds.end()) {
                    cardsToRemove.push_back(cv);
                }
            }
        }
    }
    
    // 移除不需要的CardView
    for (auto* cv : cardsToRemove) {
        if (cv) {
            Node* parent = cv->getParent();
            if (parent == playFieldView) {
                playFieldView->removeCardView(cv);
                // 从GameView的内部列表移除
                auto fieldIt = std::find(_gameView->_fieldCardViews.begin(), _gameView->_fieldCardViews.end(), cv);
                if (fieldIt != _gameView->_fieldCardViews.end()) {
                    _gameView->_fieldCardViews.erase(fieldIt);
                }
            } else if (parent == stackView) {
                stackView->removeCardView(cv);
                // 从GameView的内部列表移除
                auto stackIt = std::find(_gameView->_stackCardViews.begin(), _gameView->_stackCardViews.end(), cv);
                if (stackIt != _gameView->_stackCardViews.end()) {
                    _gameView->_stackCardViews.erase(stackIt);
                }
            }
            CCLOG("GameController::undo: Removed CardView for card ID %d", cv->getModel() ? cv->getModel()->getId() : -1);
        }
    }
    
    // 4. 找出需要创建的CardView（Model中有但View中没有）
    // 注意：必须使用恢复后的GameModel中的CardModel指针，而不是快照中的
    for (auto* cardModel : _gameModel->getPlayfieldCards()) {
        if (cardModel && currentViewCardIds.find(cardModel->getId()) == currentViewCardIds.end()) {
            // 需要创建CardView，使用恢复后的GameModel中的CardModel指针
            CardView* cardView = CardView::create(cardModel);
            if (cardView && playFieldView) {
                // 从快照中获取位置信息
                auto posIt = snapshot->cardPositions.find(cardModel->getId());
                if (posIt != snapshot->cardPositions.end()) {
                    const CardPositionInfo& posInfo = posIt->second;
                    Vec2 localPos = playFieldView->convertToNodeSpace(posInfo.worldPosition);
                    cardView->setPosition(localPos);
                } else {
                    // 如果没有位置信息，使用CardModel中的位置
                    Vec2 jsonPos = cardModel->getPosition();
                    Size cardSize = cardView->getContentSize();
                    Vec2 worldPos = Vec2(jsonPos.x + cardSize.width / 2.0f, jsonPos.y + cardSize.height / 2.0f);
                    Vec2 localPos = playFieldView->convertToNodeSpace(worldPos);
                    cardView->setPosition(localPos);
                }
                playFieldView->addCardView(cardView);
                // 添加到GameView的内部列表
                _gameView->_fieldCardViews.push_back(cardView);
                CCLOG("GameController::undo: Created CardView for PlayField card ID %d (Model ptr: %p)", 
                      cardModel->getId(), cardModel);
            }
        }
    }
    
    for (auto* cardModel : _gameModel->getStackCards()) {
        if (cardModel && currentViewCardIds.find(cardModel->getId()) == currentViewCardIds.end()) {
            // 需要创建CardView，使用恢复后的GameModel中的CardModel指针
            CardView* cardView = CardView::create(cardModel);
            if (cardView && stackView) {
                // 从快照中获取位置信息
                auto posIt = snapshot->cardPositions.find(cardModel->getId());
                if (posIt != snapshot->cardPositions.end()) {
                    const CardPositionInfo& posInfo = posIt->second;
                    Vec2 localPos = stackView->convertToNodeSpace(posInfo.worldPosition);
                    cardView->setPosition(localPos);
                } else {
                    // 如果没有位置信息，使用CardModel中的位置
                    Vec2 worldPos = cardModel->getPosition();
                    Vec2 localPos = stackView->convertToNodeSpace(worldPos);
                    cardView->setPosition(localPos);
                }
                stackView->addCardView(cardView);
                // 添加到GameView的内部列表
                _gameView->_stackCardViews.push_back(cardView);
                CCLOG("GameController::undo: Created CardView for Stack card ID %d (Model ptr: %p)", 
                      cardModel->getId(), cardModel);
            }
        }
    }
    
    // 5. 根据快照中的位置信息，播放回退动画（移动现有卡牌）
    for (const auto& posPair : snapshot->cardPositions) {
        int cardId = posPair.first;
        const CardPositionInfo& posInfo = posPair.second;
        
        // 查找对应的CardView
        CardView* cardView = _gameView->findCardViewByModelId(cardId);
        if (cardView) {
            // 播放回退动画：移动到记录的位置
            _gameView->playUndoAnimation(cardView, posInfo.worldPosition, posInfo.isInStack);
        }
    }
    
    CCLOG("GameController::undo: View restoration complete");
    
    // 更新回退按钮状态
    if (_gameView && _gameView->getStackView()) {
        _gameView->getStackView()->updateUndoButtonState(this->canUndo());
    }
    
    return true;
}

bool GameController::canUndo() const
{
    return _undoManager ? _undoManager->canUndo() : false;
}

void GameController::recordUndo()
{
    if (!_undoManager || !_gameView) {
        return;
    }
    
    // 收集所有卡牌的位置信息
    std::map<int, CardPositionInfo> cardPositions = _gameView->collectCardPositions();
    
    // 记录到UndoManager
    _undoManager->record(cardPositions);
    
    CCLOG("GameController::recordUndo: Recorded %d card positions", static_cast<int>(cardPositions.size()));
}

