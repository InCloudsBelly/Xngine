#pragma once

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Platform/OpenGL/OpenGLTexture.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <set>
// TODO: REMOVE!
typedef unsigned int GLenum;

namespace X
{


	class NativeOpenGLShader :public Shader
	{
    public:
		NativeOpenGLShader(const std::string& filepath);
		NativeOpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~NativeOpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetBool(const std::string& name, bool value) override;
        virtual void SetInt(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        virtual void SetFloat(const std::string& name, float value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

        virtual void setImage(unsigned int imageBindUnit, Ref<Texture2D> tex) override;

        virtual const std::string& GetName() const override { return mName; };

        void UploadUniformInt(const std::string& name, int value);
        void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

        virtual void GetLocalGroupSize(std::vector<int>& localGroupSize) override;
        virtual void ComputeDispatch(int globalSize0, int globalSize1, int globalSize2)override;
        virtual void setBarrier(BarrierType type)override;

    private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

    private:
        uint32_t mRendererID;
        std::string mName;
        std::set<Ref<Texture2D>> mBindingImageTextureSet;
	};
}