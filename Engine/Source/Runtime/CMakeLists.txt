file(GLOB_RECURSE RuntimeHeaderFiles CONFIGUE_DEPENDS "*.h")
file(GLOB_RECURSE RuntimeSourceFiles CONFIGUE_DEPENDS "*.cpp")

source_group(TREE "${EngineRuntimeDir}" FILES ${RuntimeHeaderFiles} ${RuntimeSourceFiles})

add_library(XngineRuntime STATIC ${RuntimeHeaderFiles} ${RuntimeSourceFiles})

target_include_directories(XngineRuntime 
    PRIVATE "${EngineSourceDir}"
    PRIVATE "${EngineRuntimeDir}" # make sure we can directly include "Xpch.h"
    PUBLIC ${vulkan_include}
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
    PUBLIC BulletSoftBody
    PUBLIC Bullet3Dynamics 
    PUBLIC BulletDynamics 
    PUBLIC Bullet3Collision 
    PUBLIC BulletCollision 
    PUBLIC Bullet3Common 
    PUBLIC LinearMath 
    PUBLIC shaderc
    PUBLIC spirv-cross-core
    PUBLIC spirv-cross-glsl
    PUBLIC headers
    PUBLIC ${vulkan_lib}
)

if (WIN32)
    target_link_libraries(XngineRuntime PUBLIC d3d11.lib dxgi.lib dxguid.lib D3DCompiler.lib d2d1.lib dwrite.lib winmm.lib)
endif()

# if (DirectX11_FOUND)
#     target_link_libraries(XngineRuntime PUBLIC ${DirectX11_LIBRARIES})
#     target_include_directories(XngineRuntime PUBLIC ${DirectX11_INCLUDE_DIRS})
# endif()



