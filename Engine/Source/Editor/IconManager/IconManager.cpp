#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Editor/IconManager/IconManager.h"

namespace X
{
    IconManager::IconManager()
    {
        mNullTexture = Texture2D::Create(AssetManager::GetFullPath("Assets/Textures/DefaultTexture.png"));
        mDirectoryIcon = Texture2D::Create(AssetManager::GetFullPath("Resources/Icons/ContentBrowser/DirectoryIcon.png"));
        mFileIcon = Texture2D::Create(AssetManager::GetFullPath("Resources/Icons/ContentBrowser/FileIcon.png"));
        mSettingIcon = Texture2D::Create(AssetManager::GetFullPath("Resources/Icons/SettingIcon.png"));
    }
}