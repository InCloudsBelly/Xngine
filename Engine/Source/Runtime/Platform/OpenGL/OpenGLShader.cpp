#include "Xpch.h"

#include "Runtime/Resource/AssetManager/AssetManager.h"
#include "Runtime//Resource/ConfigManager/ConfigManager.h"
#include "Runtime/Platform/OpenGL/OpenGLTexture.h"
#include "Runtime/Platform/OpenGL/OpenGLShader.h"
#include "Runtime/Core/Timer.h"

#include <fstream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>


#include <shaderc/shaderc.hpp>
#include <SPIRV-Cross/spirv_cross.hpp>
#include <SPIRV-Cross/spirv_glsl.hpp>

namespace X
{
	namespace Utils
	{
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			if (type == "compute")
				return GL_COMPUTE_SHADER;

			X_CORE_ASSERT(false, "Unknown shader type!");
			return 0;
		}

		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			}
			X_CORE_ASSERT(false);
			return (shaderc_shader_kind)0;
		}

		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:   return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			}
			X_CORE_ASSERT(false);
			return nullptr;
		}

		static const char* GetCacheDirectory()
		{
			// TODO: make sure the assets directory is valid
			return "Shaders/Caches/OpenGL";
		}

		static void CreateCacheDirectoryIfNeeded()
		{
			std::string cacheDirectory = AssetManager::GetFullPath(GetCacheDirectory()).string();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}

		static const char* GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_opengl.frag";
			}
			X_CORE_ASSERT(false);
			return "";
		}

		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			case GL_COMPUTE_SHADER:   return ".cached_vulkan.comp";
			}
			X_CORE_ASSERT(false);
			return "";
		}

		static ShaderUniformType SPIRTypeToShaderUniformType(spirv_cross::SPIRType type)
		{
			switch (type.basetype)
			{
			case spirv_cross::SPIRType::Boolean:  return ShaderUniformType::Bool;
			case spirv_cross::SPIRType::Int:      return ShaderUniformType::Int;
			case spirv_cross::SPIRType::UInt:     return ShaderUniformType::UInt;
			case spirv_cross::SPIRType::Float:
				if (type.vecsize == 1)            return ShaderUniformType::Float;
				if (type.vecsize == 2)            return ShaderUniformType::Vec2;
				if (type.vecsize == 3)            return ShaderUniformType::Vec3;
				if (type.vecsize == 4)            return ShaderUniformType::Vec4;

				if (type.columns == 3)            return ShaderUniformType::Mat3;
				if (type.columns == 4)            return ShaderUniformType::Mat4;
				break;
			}
			X_CORE_ASSERT(false, "Unknown type!");
			return ShaderUniformType::None;
		}
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
		: mFilePath(filepath)
	{
		Utils::CreateCacheDirectoryIfNeeded();

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);
		
		{
			Timer timer;
			std::unordered_map<uint32_t, std::vector<uint32_t>> shaderData;
			CompileOrGetVulkanBinaries(shaderData, shaderSources);
			CompileOrGetOpenGLBinaries(shaderData, shaderSources);
			//CreateProgram();
			X_CORE_WARN("Shader creation took {0} ms", timer.ElapsedMillis());
		}

		// Extract name from filepath
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		mName = filepath.substr(lastSlash, count);
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: mName(name)
	{
		std::unordered_map<GLenum, std::string> sources;
		sources[GL_VERTEX_SHADER] = vertexSrc;
		sources[GL_FRAGMENT_SHADER] = fragmentSrc;
		
		//CompileOrGetVulkanBinaries(sources);
		//CompileOrGetOpenGLBinaries();
		CreateProgram();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(mRendererID);
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			X_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos); //End of shader type declaration line
			X_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1; //Start of shader type name (after "#type " keyword)
			std::string type = source.substr(begin, eol - begin);
			X_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel" || type == "compute", "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
			X_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

			shaderSources[Utils::ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

		}

		return shaderSources;
	}


	void OpenGLShader::CompileOrGetVulkanBinaries(std::unordered_map<uint32_t, std::vector<uint32_t>>& outputBinary, const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();
		for (auto [stage, source] : shaderSources)
		{
			auto extension = Utils::GLShaderStageCachedVulkanFileExtension(stage);
		
			std::filesystem::path p = mFilePath;
			auto path = AssetManager::GetFullPath(cacheDirectory.string())/ (p.filename().string() + extension);
			std::string cachedFilePath = path.string();

			FILE* f = fopen(cachedFilePath.c_str(), "rb");
			if (f)
			{
				fseek(f, 0, SEEK_END);
				uint64_t size = ftell(f);
				fseek(f, 0, SEEK_SET);
				outputBinary[stage] = std::vector<uint32_t>(size / sizeof(uint32_t));
				fread(outputBinary[stage].data(), sizeof(uint32_t), outputBinary[stage].size(), f);
				fclose(f);
			}
		

			if (outputBinary[stage].size() == 0)
			{
				// Do we need to init a compiler for each stage?
				shaderc::Compiler compiler;
				shaderc::CompileOptions options;
				options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
				options.AddMacroDefinition("OPENGL");
				const bool optimize = false;
				if (optimize)
					options.SetOptimizationLevel(shaderc_optimization_level_performance);

				// Compile shader
				{
					auto& shaderSource = shaderSources.at(stage);
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(shaderSource, Utils::GLShaderStageToShaderC(stage), mFilePath.c_str(), options);

					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						X_CORE_ERROR(module.GetErrorMessage());
						X_CORE_ASSERT(false);
					}

					const uint8_t* begin = (const uint8_t*)module.cbegin();
					const uint8_t* end = (const uint8_t*)module.cend();
					const ptrdiff_t size = end - begin;

					outputBinary[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
				}

				// Cache compiled shader
				{
					std::filesystem::path p = mFilePath;
					auto path = AssetManager::GetFullPath(cacheDirectory.string()) / (p.filename().string() + extension);
					std::string cachedFilePath = path.string();

					FILE* f = fopen(cachedFilePath.c_str(), "wb");
					fwrite(outputBinary[stage].data(), sizeof(uint32_t), outputBinary[stage].size(), f);
					fclose(f);
				}
			}
		}
	}

	void OpenGLShader::CompileOrGetOpenGLBinaries(std::unordered_map<uint32_t, std::vector<uint32_t>>& vulkanBinaries, const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		if (mRendererID)
			glDeleteProgram(mRendererID);

		GLuint program = glCreateProgram();
		mRendererID = program;

		std::vector<GLuint> shaderRendererIDs;
		shaderRendererIDs.reserve(vulkanBinaries.size());

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		mConstantBufferOffset = 0;
		std::vector<std::vector<uint32_t>> shaderData;
		for (auto [stage, binary] : vulkanBinaries)
		{
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;
			options.SetTargetEnvironment(shaderc_target_env_opengl_compat, shaderc_env_version_opengl_4_5);

			{
				spirv_cross::CompilerGLSL glsl(binary);
				ParseConstantBuffers(glsl);

				std::filesystem::path p = mFilePath;
				auto path = AssetManager::GetFullPath(cacheDirectory.string()) / (p.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));
				std::string cachedFilePath = path.string();

				std::vector<uint32_t>& shaderStageData = shaderData.emplace_back();

				
				FILE* f = fopen(cachedFilePath.c_str(), "rb");
				if (f)
				{
					fseek(f, 0, SEEK_END);
					uint64_t size = ftell(f);
					fseek(f, 0, SEEK_SET);
					shaderStageData = std::vector<uint32_t>(size / sizeof(uint32_t));
					fread(shaderStageData.data(), sizeof(uint32_t), shaderStageData.size(), f);
					fclose(f);
				}


				if (!shaderStageData.size())
				{
					std::string source = glsl.compile();
#if PRINT_SHADERS
					printf("=========================================\n");
					printf("%s Shader:\n%s\n", GLShaderTypeToString(stage), source.c_str());
					printf("=========================================\n");
#endif
					shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), mFilePath.c_str(), options);

					if (module.GetCompilationStatus() != shaderc_compilation_status_success)
					{
						X_CORE_ERROR(module.GetErrorMessage());
						X_CORE_ASSERT(false);
					}

					shaderStageData = std::vector<uint32_t>(module.cbegin(), module.cend());

					{
						std::filesystem::path p = mFilePath;
						auto path = AssetManager::GetFullPath(cacheDirectory.string()) / (p.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));
						std::string cachedFilePath = path.string();
						FILE* f = fopen(cachedFilePath.c_str(), "wb");
						fwrite(shaderStageData.data(), sizeof(uint32_t), shaderStageData.size(), f);
						fclose(f);
					}
				}

				GLuint shaderID = glCreateShader(stage);
				glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderStageData.data(), static_cast<GLsizei>(shaderStageData.size()) * sizeof(uint32_t));
				glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
				glAttachShader(program, shaderID);

				shaderRendererIDs.emplace_back(shaderID);
			}
		}

		// Link shader program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			X_CORE_ERROR("Shader compilation failed ({0}):\n{1}", mFilePath, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : shaderRendererIDs)
				glDeleteShader(id);
		}

		mRendererID = program;

		//// Always detach shaders after a successful link.
		//for (auto id : shaderRendererIDs)
		//	glDetachShader(program, id);

		//// Get uniform locations
		//for (auto& [bufferName, buffer] : mBuffers)
		//{
		//	for (auto& [name, uniform] : buffer.Uniforms)
		//	{
		//		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		//		if (location == -1)
		//			X_CORE_WARN("{0}: could not find uniform location {0}", name);

		//		mUniformLocations[name] = location;
		//	}
		//}

		//for (auto& shaderStageData : shaderData)
		//{
		//	Reflect(shaderStageData);
		//}
	}

	//void OpenGLShader::CompileOrGetOpenGLBinaries()
	//{
	//	auto& shaderData = mOpenGLSPIRV;

	//	shaderc::Compiler compiler;
	//	shaderc::CompileOptions options;
	//	options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
	//	options.AddMacroDefinition("OPENGL");

	//	const bool optimize = false;
	//	if (optimize)
	//		options.SetOptimizationLevel(shaderc_optimization_level_performance);

	//	std::filesystem::path cacheDirectory = AssetManager::GetFullPath(Utils::GetCacheDirectory());

	//	shaderData.clear();
	//	mOpenGLSourceCode.clear();

	//	int binding = 0;

	//	for (auto&& [stage, spirv] : mVulkanSPIRV)
	//	{
	//		spirv_cross::CompilerGLSL glslCompiler(spirv);
	//		ParseConstantBuffers(glslCompiler);

	//		std::filesystem::path shaderFilePath = mFilePath;
	//		std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() + Utils::GLShaderStageCachedOpenGLFileExtension(stage));

	//		std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
	//		if (in.is_open())
	//		{
	//			in.seekg(0, std::ios::end);
	//			auto size = in.tellg();
	//			in.seekg(0, std::ios::beg);

	//			auto& data = shaderData[stage];
	//			data.resize(size / sizeof(uint32_t));
	//			in.read((char*)data.data(), size);
	//		}
	//		else
	//		{
	//			// from https://github.com/KhronosGroup/SPIRV-Cross/issues/887
	//			auto resources = glslCompiler.get_shader_resources();
	//			glslCompiler.set_decoration(resources.push_constant_buffers.front().id, spv::DecorationLocation, binding);
	//			// end from

	//			mOpenGLSourceCode[stage] = glslCompiler.compile();
	//			auto& source = mOpenGLSourceCode[stage];

	//			shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), mFilePath.c_str(), options);
	//			if (module.GetCompilationStatus() != shaderc_compilation_status_success)
	//			{
	//				X_CORE_ERROR(module.GetErrorMessage());
	//				X_CORE_ASSERT(false);
	//			}

	//			shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());

	//			std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
	//			if (out.is_open())
	//			{
	//				auto& data = shaderData[stage];
	//				out.write((char*)data.data(), data.size() * sizeof(uint32_t));
	//				out.flush();
	//				out.close();
	//			}
	//			binding++;
	//		}

	//	}

	//	// Get uniform locations
	//	for (auto& [bufferName, buffer] : mBuffers)
	//	{
	//		for (auto& [name, uniform] : buffer.Uniforms)
	//		{
	//			GLint location = glGetUniformLocation(mRendererID, name.c_str());
	//			if (location == -1)
	//				X_CORE_WARN("{0}: could not find uniform location {0}", name);

	//			mUniformLocations[name] = location;
	//		}
	//	}
	//}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();

		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : mOpenGLSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);

		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			X_CORE_ERROR("Shader linking failed ({0}):\n{1}", mFilePath, infoLog.data());

			glDeleteProgram(program);

			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}

		mRendererID = program;
	}


	void OpenGLShader::ParseConstantBuffers(const spirv_cross::CompilerGLSL& compiler)
	{
		// TODO: Still have some bugs

		spirv_cross::ShaderResources res = compiler.get_shader_resources();
		for (const spirv_cross::Resource& resource : res.push_constant_buffers)
		{
			const auto& bufferName = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			auto bufferSize = compiler.get_declared_struct_size(bufferType);

			// Skip empty push constant buffers - these are for the renderer only
			//if (bufferName.empty() || bufferName == "u_Renderer")
			//{
			//	mConstantBufferOffset += static_cast<uint32_t>(bufferSize);
			//	continue;
			//}

			auto location = compiler.get_decoration(resource.id, spv::DecorationLocation);
			int memberCount = static_cast<int>(bufferType.member_types.size());
			ShaderBuffer& buffer = mBuffers[bufferName];
			buffer.Name = bufferName;
			buffer.Size = static_cast<uint32_t>(bufferSize) - mConstantBufferOffset;
			for (int i = 0; i < memberCount; i++)
			{
				auto type = compiler.get_type(bufferType.member_types[i]);
				const auto& memberName = compiler.get_member_name(bufferType.self, i);
				auto size = compiler.get_declared_struct_member_size(bufferType, i);
				auto offset = compiler.type_struct_member_offset(bufferType, i) - mConstantBufferOffset;

				std::string uniformName = bufferName + "." + memberName;
				buffer.Uniforms[uniformName] = ShaderUniform(uniformName, Utils::SPIRTypeToShaderUniformType(type), static_cast<uint32_t>(size), offset);
			}

			mConstantBufferOffset += static_cast<uint32_t>(bufferSize);
		}
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		X_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), mFilePath);
		X_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
		X_CORE_TRACE("    {0} resources", resources.sampled_images.size());

		X_CORE_TRACE("Uniform buffers:");
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			X_CORE_TRACE("    Name = {0}", resource.name.c_str());
			X_CORE_TRACE("    Size = {0}", bufferSize);
			X_CORE_TRACE("    Binding = {0}", binding);
			X_CORE_TRACE("    Members = {0}", memberCount);
		}

		X_CORE_TRACE("Uniform variables:");
		for (const auto& resource : resources.push_constant_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			X_CORE_TRACE("    Name = {0}", resource.name.c_str());
			X_CORE_TRACE("    Name = {0}", compiler.get_name(resource.base_type_id));
			X_CORE_TRACE("    Name = {0}", compiler.get_fallback_name(resource.base_type_id));
			//X_CORE_TRACE("    Name = {0}", compiler.get_name(resource.base_type_id));
			X_CORE_TRACE("    Size = {0}", bufferSize);
			X_CORE_TRACE("    Binding = {0}", binding);
			X_CORE_TRACE("    Members = {0}", memberCount);

			// from https://github.com/KhronosGroup/SPIRV-Cross/wiki/Reflection-API-user-guide
			auto& type = compiler.get_type(resource.base_type_id);
			unsigned member_count = type.member_types.size();
			for (unsigned i = 0; i < member_count; i++)
			{
				auto& member_type = compiler.get_type(type.member_types[i]);
				size_t member_size = compiler.get_declared_struct_member_size(type, i);

				// Get member offset within this struct.
				size_t offset = compiler.type_struct_member_offset(type, i);

				if (!member_type.array.empty())
				{
					// Get array stride, e.g. float4 foo[]; Will have array stride of 16 bytes.
					size_t array_stride = compiler.type_struct_member_array_stride(type, i);
				}

				if (member_type.columns > 1)
				{
					// Get bytes stride between columns (if column major), for float4x4 -> 16 bytes.
					size_t matrix_stride = compiler.type_struct_member_matrix_stride(type, i);
				}
				const std::string& name = compiler.get_member_name(type.self, i);

				X_CORE_TRACE("    Name = {0}", name);
			}
			// end from
		}
	}


	void OpenGLShader::Bind() const
	{
		glUseProgram(mRendererID);
	}

	void OpenGLShader::Unbind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::SetBool(const std::string& name, bool value)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count)
	{
		UploadUniformIntArray(name, values, count);
	}

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		UploadUniformMat4(name, value);
	}

	void OpenGLShader::SetUniform(const std::string& fullname, const glm::mat4& value)
	{
		X_CORE_ASSERT(mUniformLocations.find(fullname) != mUniformLocations.end());
		GLint location = mUniformLocations.at(fullname);
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}


	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1iv(location, count, values);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform2f(location, value.x, value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(mRendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}
