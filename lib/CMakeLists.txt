
# other libs

find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

# imgui

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW=1)

file(GLOB IMGUI_SOURCES imgui/*.cpp)

set(IMGUI_BACKEND_SOURCES
	imgui/backends/imgui_impl_opengl3.cpp
	imgui/backends/imgui_impl_glfw.cpp)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})
target_include_directories(imgui PUBLIC imgui)
target_link_libraries(imgui ${GLEW_LIBRARIES})

add_library(imguifilepicker STATIC ImGuiFileDialog/ImGuiFileDialog.cpp)
target_include_directories(imguifilepicker PUBLIC imgui)