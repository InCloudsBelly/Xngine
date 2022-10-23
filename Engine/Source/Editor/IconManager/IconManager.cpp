#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Editor/IconManager/IconManager.h"

namespace X
{
    IconManager::IconManager()
    {
        Add("EntityIcon", Texture2D::Create(AssetManager::GetFullPath("Resources/Icons/Entity2.png")));
        Add("NullTextrue", Texture2D::Create(AssetManager::GetFullPath("Assets/Textures/DefaultTexture.png")));
        Add("SettingIcon", Texture2D::Create(std::string(("Resources/Icons/SettingIcon.png"))));
        Add("DirectoryIcon", Texture2D::Create(std::string(("Resources/Icons/ContentBrowser/DirectoryIcon.png"))));
        Add("FileIcon", Texture2D::Create(std::string(("Resources/Icons/ContentBrowser/FileIcon.png"))));
        Add("PlayIcon", Texture2D::Create(std::string(("Resources/Icons/PlayButton.png"))));
        Add("StopIcon", Texture2D::Create(std::string(("Resources/Icons/StopButton.png"))));
        Add("TransIcon", Texture2D::Create(std::string(("Resources/Icons/Trans.png"))));
        Add("RotateIcon", Texture2D::Create(std::string(("Resources/Icons/Rotate.png"))));
        Add("ScaleIcon", Texture2D::Create(std::string(("Resources/Icons/Scale.png"))));
    }
}