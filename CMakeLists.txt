cmake_minimum_required(VERSION 3.10)
project(cane)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_subdirectory(lib)

add_executable(cane
    src/main.cpp
    src/Editor.cpp

    src/Gui/DebugInformation.cpp
    src/Gui/ModelBrowser.cpp
    src/Gui/ModelViewer.cpp
    src/Gui/Renderer.cpp
    src/Gui/RenderBuffer.cpp
    src/Gui/RendererOptions.cpp 

    src/Renderer/Camera.cpp
    src/Renderer/Cube.cpp
    src/Renderer/Object.cpp
    src/Renderer/OBJModel.cpp
    src/Renderer/Shader.cpp

    src/Structs/sly_level_structs.cpp
    src/Structs/SlyLevelFile.cpp

    src/Utility/math.cpp
    src/Utility/config.cpp
    src/Utility/FileReader.cpp
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(cane PRIVATE -O3 -Wall -Wextra -Wpedantic -Werror)
else()
    target_compile_options(cane PRIVATE -g)
endif()

target_include_directories(cane PRIVATE src)
target_include_directories(cane PRIVATE lib)

set_property(TARGET cane PROPERTY CXX_STANDARD 26)

add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Shaders/colored_object_shader_fragment.glsl      $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Shaders/colored_object_shader_vertex.glsl        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Shaders/colored_sly_object_shader_fragment.glsl  $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Shaders/colored_sly_object_shader_vertex.glsl    $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Shaders/lit_object_shader_fragment.glsl          $<TARGET_FILE_DIR:${PROJECT_NAME}>)
add_custom_command(TARGET cane POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/Shaders/lit_object_shader_vertex.glsl            $<TARGET_FILE_DIR:${PROJECT_NAME}>)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(cane Threads::Threads)

target_link_libraries(cane ${GLFW_LIBRARIES})
target_link_libraries(cane ${GLEW_LIBRARIES})
target_link_libraries(cane ${GLM_LIBRARIES})
target_link_libraries(cane GL)

target_link_libraries(cane imgui)
target_link_libraries(cane imguifilepicker)
