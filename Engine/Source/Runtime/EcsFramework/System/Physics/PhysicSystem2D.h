#pragma once

#include "Runtime/EcsFramework/System/System.h"
#include "Runtime/EcsFramework/Level/Level.h"

namespace X
{
	class PhysicSystem2D : public System
	{
	public:
		PhysicSystem2D(Level* level) : System(level) {}
		virtual ~PhysicSystem2D() = default;
	public:
		void OnRuntimeStart() override;
		void OnUpdateRuntime(Timestep ts) override;
		void OnRuntimeStop() override;
	private:
		class b2World* mPhysicsWorld = nullptr;
	};
}