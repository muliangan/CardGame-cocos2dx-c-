# CardGame-cocos2dx
a cardgamedome by cocos2dx
使用方法，新建一个cocos2dx文件，点击进入.sln文件，然后将class的除了MainmenuScene的.cpp和.h文件添加进入右侧的src里，将资源文件导入cocos2dx项目目录下的Resources文件

Classes/

├── AppDelegate.h/cpp # 应用程序入口和生命周期管理

├── MainMenuScene.h/cpp # 主菜单场景

│

├── scene/ # 游戏场景层

│ ├── GameScene.h/cpp # 游戏主场景

│ └── CardTestScene.h/cpp # 卡牌测试场景（调试用）

│

├── models/ # 数据模型层（运行时动态数据）

│ ├── CardModel.h/cpp # 卡牌数据模型

│ └── GameModel.h/cpp # 游戏状态数据模型

│

├── views/ # 视图层（UI显示和用户交互）

│ ├── CardView.h/cpp # 卡牌视图（单个卡牌的显示）

│ ├── GameView.h/cpp # 游戏视图

│ ├── PlayFieldView.h/cpp # 游戏区域视图（field部分）

│ └── StackView.h/cpp # 堆叠区域视图（stack部分）

│

├── controllers/ # 控制器层（业务逻辑）

│ ├── GameController.h/cpp # 游戏主控制器

│ ├── PlayFieldController.h/cpp # field区域控制器

│ ├── StackController.h/cpp # stack区域控制器

│

├── configs/ # 静态配置相关类

│ ├── ConfigManager.h/cpp # 配置管理器

│ ├── loaders/ # 配置加载器

│ │ └── LevelConfigLoader.h/cpp # 关卡配置加载器（从JSON加载）

│ └── models/ # 配置数据模型

│  |  ├── LevelConfig.h # 关卡配置结构体

│  | ├── CardResConfig.h/cpp # 卡牌资源配置

│  |  └── CardResConfigManager.h/cpp # 卡牌资源配置管理器

│

├── managers/ # 管理器层

│ └── UndoManager.h/cpp # 撤销管理器（管理操作历史）

│

├── services/ # 服务层

│ └── GameModelFromLevelGenerator.h/cpp # 游戏模型生成服务

│

└── utils/ # 工具类（待扩展）

1.如何新增一个卡牌类型\
可以扩展CardModel数据
，可以新增一个卡牌类型
- enum CardType {\
    CT_NORMAL = 0,\
    NEW，\
    NEW，\
};\
在卡牌类里添加一个新的私有属性cardtype

扩展CardView,新增一个特殊牌显示的函数即可

2.一个新类型的回退功能\
可以新增一个按钮将功能和新的回退功能,在GameController添加新功能的调用接口



demo视频文件
https://github.com/user-attachments/assets/ec192ebc-d7b9-49db-af4b-91f33f937837


