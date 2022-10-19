file(GLOB_RECURSE RuntimeHeaderFiles CONFIGUE_DEPENDS "*.h")
file(GLOB_RECURSE RuntimeSourceFiles CONFIGUE_DEPENDS "*.cpp")

source_group(TREE "${EngineRuntimeDir}" FILES ${RuntimeHeaderFiles} ${RuntimeSourceFiles})

add_library(XngineRuntime STATIC ${RuntimeHeaderFiles} ${RuntimeSourceFiles})

target_include_directories(XngineRuntime 
    PRIVATE "${EngineSourceDir}"
    PRIVATE "${EngineRuntimeDir}" # make sure we can directly include "Xpch.h"
)

target_compile_definitions(XngineRuntime
    PRIVATE X_PLATFORM_WINDOWS
    PRIVATE X_BUILD_DLL
    PRIVATE GLFW_INCLUDE_NONE
    PRIVATE IMGUI_IMPL_OPENGL_LOADER_CUSTOM
    PRIVATE X_DEBUG
    "ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

target_precompile_headers(XngineRuntime PUBLIC "Xpch.h")

target_link_libraries(XngineRuntime PUBLIC glfw
    PUBLIC Glad
    PUBLIC glm
    PUBLIC imgui
    PUBLIC ${OPENGL_LIBRARIES}
    PUBLIC yaml-cpp
    PUBLIC spdlog
    PUBLIC Box2D
    PUBLIC stb
    PUBLIC ImGuizmo
    PUBLIC assimp
    PUBLIC shaderc
    PUBLIC spirv-cross-core
    PUBLIC spirv-cross-glsl
    PUBLIC headers
)



