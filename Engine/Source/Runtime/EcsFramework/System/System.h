#pragma once

#include "Runtime/Core/Timestep.h"
#include "Runtime/EcsFramework/Level/Level.h"

namespace X
{
	class System
	{
	public:
		System() = delete;
		System(Level* level) : mLevel(level) {}
		virtual ~System() = default;

	public:
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnRuntimeStart() {}
		virtual void OnRuntimeStop() {}

	protected:
		Level* mLevel = nullptr;
	};
}