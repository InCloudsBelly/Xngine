#pragma once

#include "Runtime/Renderer/Shader.h"
#include "Runtime/Renderer/Texture.h"
#include <glm/glm.hpp>
#include <unordered_map>

#include <SPIRV-Cross/spirv_glsl.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;
typedef int GLint;

namespace X
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetBool(const std::string& name, bool value) override;
        virtual void SetInt(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;
        virtual void SetFloat(const std::string& name, float value) override;
        virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
        virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
        virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
        virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
        virtual void setImage(unsigned int imageBindUnit, Ref<Texture2D> tex) override {};
        
        virtual void SetUniform(const std::string& fullname, const glm::mat4& value) override;

        virtual const std::string& GetName() const override { return mName; };

        virtual void GetLocalGroupSize(std::vector<int>& localGroupSize) override {};
        virtual void ComputeDispatch(int globalSize0, int globalSize1, int globalSize2)override {};
        virtual void setBarrier(BarrierType type)override {};

        void UploadUniformInt(const std::string& name, int value);
        void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

        void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);
    private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void ParseConstantBuffers(const spirv_cross::CompilerGLSL& compiler);

        void CompileOrGetVulkanBinaries(std::unordered_map<uint32_t, std::vector<uint32_t>>& outputBinary, const std::unordered_map<GLenum, std::string>& shaderSources);
        //void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
        void CompileOrGetOpenGLBinaries(std::unordered_map<uint32_t, std::vector<uint32_t>>& vulkanBinaries, const std::unordered_map<GLenum, std::string>& shaderSources);
        //void CompileOrGetOpenGLBinaries();
        void CreateProgram();
        void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);
    private:
        uint32_t mRendererID;
        std::string mFilePath;
        std::string mName;
        std::unordered_map<GLenum, std::vector<uint32_t>> mVulkanSPIRV;
        std::unordered_map<GLenum, std::vector<uint32_t>> mOpenGLSPIRV;

        std::unordered_map<GLenum, std::string> mOpenGLSourceCode;

        uint32_t mConstantBufferOffset = 0;

        std::unordered_map<std::string, ShaderBuffer> mBuffers;
        std::unordered_map<std::string, GLint> mUniformLocations;
    };
}