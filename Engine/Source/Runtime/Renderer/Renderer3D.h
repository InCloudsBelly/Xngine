#pragma once
#include "Runtime/Camera/EditorCamera.h"
#include "Runtime/EcsFramework/Level/Level.h"
#include "Runtime/EcsFramework/Component/Mesh/MeshComponent.h"
#include "Runtime/Renderer/Texture.h"
#include "RenderPass.h"
#include "Pipeline.h"

namespace X
{
    class Renderer3D
    {
    public:
        static void Init();
        static void Shutdown();

        static void DrawModel(const glm::mat4& transform, const glm::vec3& cameraPos, MeshComponent& MeshComponent, int EntityID);

        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void BeginScene(const EditorCamera& camera);
        static void EndScene();

    public:
        static void DefaultDrawMesh(Level* level, Ref<Pipeline> pipeline);


    public:

        static Ref<Pipeline> lightPipeline ;
        static Ref<Pipeline> GeometryPipeline;
        static Ref<Pipeline> QuadPipeline;

        static Ref<Pipeline> HBAOGbufferPipeline;
        static Ref<Texture2D> TextureConfigNoise;
        static Ref<Pipeline> HBAOPipeline;
        static Ref<Pipeline> HBAOBlurPipeline;
        static Ref<Pipeline> HBAOQuadPipeline;
    };
}