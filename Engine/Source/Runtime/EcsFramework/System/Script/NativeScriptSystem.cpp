#include "Xpch.h"

#include "Runtime/EcsFramework/System/Script/NativeScriptSystem.h"
#include "Runtime/EcsFramework/Entity/Entity.h"
#include "Runtime/EcsFramework/Component/ComponentGroup.h"
#include "Runtime/EcsFramework/Entity/ScriptableEntity.h"

namespace X
{
	void NativeScriptSystem::OnUpdateRuntime(Timestep ts)
	{

		mLevel->mRegistry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
			{
				// TODO: Move to Level::OnScenePlay
				if (!nsc.Instance)
				{
					nsc.Instance = nsc.InstantiateScript();
					nsc.Instance->mEntity = Entity{ entity, mLevel };
					nsc.Instance->OnCreate();
				}
				nsc.Instance->OnUpdate(ts);
			});
	}
}