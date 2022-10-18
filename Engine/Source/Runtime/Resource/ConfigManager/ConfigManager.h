#pragma once

#include "Runtime/Core/Base/PublicSingleton.h"

#include <filesystem>

namespace X
{
    class ConfigManager final : public PublicSingleton<ConfigManager>
    {
        friend class PublicSingleton<ConfigManager>;

    public:
        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;
        ConfigManager() = default;

    public:
        void Initialize();

        void Clear();

        [[nodiscard]] const std::filesystem::path& GetRootFolder() const;
        [[nodiscard]] const std::filesystem::path& GetAssetsFolder() const;
        [[nodiscard]] const std::filesystem::path& GetShadersFolder() const;
        [[nodiscard]] const std::filesystem::path& GetResourcesFolder() const;

    private:
        std::filesystem::path mRootFolder;
        std::filesystem::path mAssetsFolder;
        std::filesystem::path mShadersFolder;
        std::filesystem::path mResourcesFolder;
    };
}