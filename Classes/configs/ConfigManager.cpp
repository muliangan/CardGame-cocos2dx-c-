#include "ConfigManager.h"
#include "cocos2d.h"

using namespace cocos2d;
#if 0
std::string ConfigManager::loadJsonFile(const std::string& filePath)
{
    auto fu = FileUtils::getInstance();
    std::string fullPath = fu->fullPathForFilename(filePath);

    if (!fu->isFileExist(fullPath)) {
        CCLOG("❌ ConfigManager: file not found -> %s", filePath.c_str());
        return "";
    }

    std::string content = fu->getStringFromFile(fullPath);

    if (content.empty()) {
        CCLOG("❌ ConfigManager: read failed -> %s", fullPath.c_str());
    }
    else {
        CCLOG("✔ Config Loaded %s (%lu bytes)", filePath.c_str(), content.size());
    }

    return content;
}

bool ConfigManager::fileExists(const std::string& filePath)
{
    return FileUtils::getInstance()->isFileExist(filePath);
}
#endif