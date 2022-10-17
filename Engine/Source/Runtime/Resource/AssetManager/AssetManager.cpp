#include "Xpch.h"
#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime/Resource/ConfigManager/ConfigManager.h"

namespace X
{
    std::filesystem::path AssetManager::GetFullPath(const std::string& RelativePath) const
    {
        return ConfigManager::GetInstance().GetRootFolder() / RelativePath;
    }
}