#pragma once

#include "Runtime/Renderer/Texture.h"

namespace X
{
    class SceneSettingsPanel
    {
    public:
        SceneSettingsPanel();

        void OnImGuiRender(bool* pOpen);

    private:
        std::vector<std::string> mPaths;

        //Visualization
        Ref<Texture> mRight;
        Ref<Texture> mLeft;
        Ref<Texture> mTop;
        Ref<Texture> mBottom;
        Ref<Texture> mBack;
        Ref<Texture> mFront;
    };
}
