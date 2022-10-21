file(GLOB EditorHeaderFiles CONFIGUE_DEPENDS "*.h" "Panels/*.h" "ImGuiWrapper/*.h" "IconManager/*.h")
file(GLOB EditorSourceFiles CONFIGUE_DEPENDS "*.cpp" "Panels/*.cpp" "ImGuiWrapper/*.cpp" "IconManager/*.cpp")

source_group(TREE ${EngineEditorDir} FILES ${EditorHeaderFiles} ${EditorSourceFiles})

add_executable(XngineEditor ${EditorHeaderFiles} ${EditorSourceFiles})

add_compile_definitions("ENGINE_ROOT_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

target_link_libraries(XngineEditor PRIVATE XngineRuntime)

target_include_directories(XngineEditor PRIVATE
	"${EngineSourceDir}"
	"${EngineRuntimeDir}"
)

target_compile_definitions(XngineEditor PRIVATE
	X_PLATFORM_WINDOWS
	X_DEBUG
	_CRT_SECURE_NO_WARNINGS
)

if(MSVC)
	set_target_properties(
	XngineEditor PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY "${EngineEditorDir}")
endif()

add_custom_command(TARGET XngineEditor POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
                   COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${EngineConfigDir}/imgui.ini" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                   COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${EngineConfigDir}/imgui.ini" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/"
)
