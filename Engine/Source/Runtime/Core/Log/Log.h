#pragma once

#include "Runtime/Core/Base/Base.h"

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace X
{
	class XNGINE_API Log
	{
	public:
		static void Init();

		[[nodiscard]] inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		[[nodiscard]] inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log macros
#define X_CORE_TRACE(...)		::X::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define X_CORE_INFO(...)		::X::Log::GetCoreLogger()->info(__VA_ARGS__)
#define X_CORE_WARN(...)		::X::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define X_CORE_ERROR(...)		::X::Log::GetCoreLogger()->error(__VA_ARGS__)
#define X_CORE_FATAL(...)		::X::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define X_TRACE(...)			::X::Log::GetClientLogger()->trace(__VA_ARGS__)
#define X_INFO(...)			::X::Log::GetClientLogger()->info(__VA_ARGS__)
#define X_WARN(...)			::X::Log::GetClientLogger()->warn(__VA_ARGS__)
#define X_ERROR(...)			::X::Log::GetClientLogger()->error(__VA_ARGS__)
#define X_FATAL(...)			::X::Log::GetClientLogger()->fatal(__VA_ARGS__)