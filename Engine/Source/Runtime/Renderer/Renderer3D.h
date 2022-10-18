#pragma once
#include "Runtime/Renderer/Camera.h"
#include "Runtime/Renderer/EditorCamera.h"
#include "Runtime/EcsFramework/Component/Mesh/StaticMeshComponent.h"

namespace X
{
    class Renderer3D
    {
    public:
        static void Init();
        static void Shutdown();

        static void DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, int EntityID);

        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void BeginScene(const EditorCamera& camera);
        static void EndScene();

    };
}