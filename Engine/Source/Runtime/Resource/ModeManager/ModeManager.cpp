#include "Xpch.h"
#include "Runtime/Resource/ModeManager/ModeManager.h"

namespace X
{
    bool ModeManager::bEditState = true;
    int ModeManager::b3DMode = 1;
    SceneMode ModeManager::mSceneMode = SceneMode::None;
    EditMode ModeManager::mEditMode = EditMode::Select;
}