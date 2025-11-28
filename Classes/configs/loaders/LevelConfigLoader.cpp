#include "../Classes/configs/loaders/LevelConfigLoader.h"
#include "../Classes/configs/models/LevelConfig.h"
#include "json/document.h"
#include "json/filereadstream.h"
#include "cocos2d.h"

using namespace cocos2d;
using namespace rapidjson;

LevelConfig LevelConfigLoader::loadLevelConfig(int levelNumber)
{
    // 根据关卡编号构建 levelId（如 1 -> "level1"）
    std::string levelId = "level" + std::to_string(levelNumber);
    return loadLevelConfig(levelId);
}

LevelConfig LevelConfigLoader::loadLevelConfig(const std::string& levelId)
{
    // 根据 levelId 构建文件路径
    std::string filePath = "../Resources/Levels/" + levelId + ".json";
    return loadFromJson(filePath);
}

//解析json函数，压入playfeildcard和stackcard栈，得到我们的每一关的卡牌有哪些
LevelConfig LevelConfigLoader::loadFromJson(const std::string& relativePath) {
    LevelConfig config;

    auto fileUtils = FileUtils::getInstance();

    if (!fileUtils->isFileExist(relativePath)) {
        CCLOG("[LevelConfigLoader] File does not exist: %s", relativePath.c_str());
        return config;
    }

    std::string content = fileUtils->getStringFromFile(relativePath);
    if (content.empty()) {
        CCLOG("[LevelConfigLoader] File is empty: %s", relativePath.c_str());
        return config;
    }

    CCLOG("[LevelConfigLoader] File content size: %d", (int)content.size());

    // 解析 JSON
    Document doc;
    doc.Parse(content.c_str());
    if (doc.HasParseError()) {
        CCLOG("[LevelConfigLoader] JSON parse error in file: %s", relativePath.c_str());
        return config;
    }

    // 解析 Playfield
    if (doc.HasMember("Playfield") && doc["Playfield"].IsArray()) {
        const auto& arr = doc["Playfield"].GetArray();
        for (auto& c : arr) {
            CardConfig card;
            card.face = c["CardFace"].GetInt();
            card.suit = c["CardSuit"].GetInt();
            card.pos = Vec2(c["Position"]["x"].GetFloat(), c["Position"]["y"].GetFloat());
            config.playfieldCards.push_back(card);
        }
    }

    // 解析 Stack
    if (doc.HasMember("Stack") && doc["Stack"].IsArray()) {
        const auto& arr = doc["Stack"].GetArray();
        for (auto& c : arr) {
            CardConfig card;
            card.face = c["CardFace"].GetInt();
            card.suit = c["CardSuit"].GetInt();
            card.pos = Vec2(c["Position"]["x"].GetFloat(), c["Position"]["y"].GetFloat());
            config.stackCards.push_back(card);
        }
    }

    CCLOG("[LevelConfigLoader] Loaded: Playfield=%d, Stack=%d",
        (int)config.playfieldCards.size(),
        (int)config.stackCards.size());

    return config;
}
