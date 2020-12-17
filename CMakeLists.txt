cmake_minimum_required(VERSION 3.10)
project(cane)

add_executable(cane
	src/main.cpp
	src/Utility/ez_stream.h
	src/Utility/FileReader.h
	src/Utility/Sigscan.h
	src/Structs/sly_level_structs.h
	src/Structs/sly_texture_structs.h
	src/Structs/SlyLevelFile.h
	src/Renderer/Camera.h
	src/Renderer/Camera.cpp
	src/Renderer/Cube.h
	src/Renderer/Cube.cpp
	src/Renderer/LightingProperties.h
	src/Renderer/Object.h
	src/Renderer/Object.cpp
	src/Renderer/OBJModel.h
	src/Renderer/OBJModel.cpp
	src/Renderer/Shader.h
	src/Renderer/Shader.cpp
)

target_include_directories(cane PUBLIC include)
target_include_directories(cane PRIVATE src)
target_include_directories(cane PRIVATE resources)

set_property(TARGET cane PROPERTY CXX_STANDARD 20)

add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/colored_object_shader_fragment.glsl		$<TARGET_FILE_DIR:${PROJECT_NAME}>	)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/colored_object_shader_vertex.glsl			$<TARGET_FILE_DIR:${PROJECT_NAME}>	)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/colored_sly_object_shader_fragment.glsl	$<TARGET_FILE_DIR:${PROJECT_NAME}>	)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/colored_sly_object_shader_vertex.glsl		$<TARGET_FILE_DIR:${PROJECT_NAME}>	)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/lit_object_shader_fragment.glsl			$<TARGET_FILE_DIR:${PROJECT_NAME}>	)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resources/lit_object_shader_vertex.glsl				$<TARGET_FILE_DIR:${PROJECT_NAME}>	)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(cane Threads::Threads)

add_library(glad lib/glad/src/glad.c)
target_include_directories(glad PRIVATE lib/glad/include)
target_include_directories(cane PUBLIC lib/glad/include)
target_link_libraries(cane glad)

add_subdirectory(lib/glfw)
target_link_libraries(cane glfw)

add_subdirectory(include/glm)
target_link_libraries(cane glm)

include_directories(include/imgui)
add_library(imgui STATIC
	include/imgui/imconfig.h
	include/imgui/imgui_demo.cpp
	include/imgui/imgui_draw.cpp
	include/imgui/imgui_internal.h
	include/imgui/imgui_widgets.cpp
	include/imgui/imgui.cpp
	include/imgui/imgui.h
	include/imgui/imstb_rectpack.h
	include/imgui/imstb_textedit.h
	include/imgui/imstb_truetype.h
)

target_link_libraries(cane imgui)
