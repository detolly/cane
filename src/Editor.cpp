#include "Editor.h"
#include <Gui/DebugInformation.h>
#include <Gui/Renderer.h>
#include <Gui/ModelBrowser.h>
#include <Gui/ModelViewer.h>
#include <Gui/RendererOptions.h>
#include <Structs/SlyLevelFile.h>
#include <Utility/dbgprint.h>
#include <cstdlib>
#include <imgui_internal.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

void Editor::create_window()
{
    set_window(glfwCreateWindow(1600, 900, "cane", NULL, NULL));
}

void Editor::init()
{
    if (!window()) {
        glfwTerminate();
        dbgprint("ERROR CREATE WINDOW");
    }

    set_title("cane");
    set_cursor_mode(GLFW_CURSOR_NORMAL);

    ModelBrowser::the().init();
    ModelViewer::the().init();
}

void Editor::open(const char* file)
{
	if (has_file_loaded())
		delete m_level;
	m_level = new SlyLevelFile(file);
	RendererOptions::the().on_load();
	DebugInformation::the().on_load();
	Renderer::the().on_load();
	ModelBrowser::the().on_load();
	ModelViewer::the().on_load();
}

void Editor::close()
{
	if (has_file_loaded())
		delete m_level;
	m_level = nullptr;
    RendererOptions::the().on_close();
    DebugInformation::the().on_close();
    Renderer::the().on_close();
    ModelBrowser::the().on_close();
    ModelViewer::the().on_close();
}

void Editor::render()
{
    using wf = ImGuiWindowFlags_;
    ImGui::SetNextWindowSize({ width(), height() }, ImGuiCond_::ImGuiCond_Always);
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::Begin("root", nullptr, wf::ImGuiWindowFlags_NoCollapse | wf::ImGuiWindowFlags_NoTitleBar | wf::ImGuiWindowFlags_NoMove | wf::ImGuiWindowFlags_NoResize | wf::ImGuiWindowFlags_NoBringToFrontOnFocus | wf::ImGuiWindowFlags_NoInputs | wf::ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open")) {
                igfd::ImGuiFileDialog::Instance()->OpenDialog("choose_file", "Choose File", ".decompressed,.bin", "");
            }
            bool disabled = !Editor::the().has_file_loaded();
            if (disabled) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
            }
            if (ImGui::MenuItem("Close")) {
                Editor::the().close();
            }
            if (disabled) {
                ImGui::PopItemFlag();
                ImGui::PopStyleVar();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("Renderer", nullptr, &config::the().windows.renderer);
            ImGui::MenuItem("Renderer Options", nullptr, &config::the().windows.renderer_options);
            ImGui::MenuItem("Debug Information", nullptr, &config::the().windows.debug_information);
            ImGui::MenuItem("Model Browser", nullptr, &config::the().windows.model_browser);
            ImGui::MenuItem("Model Viewer", nullptr, &config::the().windows.model_viewer);
            ImGui::MenuItem("Examples", nullptr, &config::the().windows.examples);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Preferences")) {

            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (igfd::ImGuiFileDialog::Instance()->FileDialog("choose_file"))
    {
        if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
        {
            std::string file_path = igfd::ImGuiFileDialog::Instance()->GetFilePathName();
            Editor::the().open(file_path.c_str());
        }

        igfd::ImGuiFileDialog::Instance()->CloseDialog("choose_file");
    }

    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoCloseButton;
    ImGuiID dockspaceID = ImGui::GetID("dock");
    //ImGui::SetNextWindowSize(ImGui::GetContentRegionAvail());
    ImGui::SetNextWindowSize({ width(), height() });
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
    if (config::the().windows.debug_information) {
        //ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
        //DebugInformation::the().render();
    }
    if (config::the().windows.renderer) {
        //ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
        Renderer::the().render();
    }
    if (config::the().windows.model_browser) {
        //ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
        ModelBrowser::the().render();
    }
    if (config::the().windows.model_viewer) {
        ModelViewer::the().render();
    }
    if (config::the().windows.renderer_options) {
        RendererOptions::the().render();
    }
    if (config::the().windows.examples) {
        ImGui::ShowDemoWindow(&config::the().windows.examples);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, (GLsizei)width(), (GLsizei)height());

    ImGui::End();
}

void Editor::size_callback(int width, int height)
{
    set_width((float)width);
    set_height((float)height);
    Renderer::the().set_should_recalculate_projection(true);
}

void Editor::handle_input()
{
    if (cursor_mode() != GLFW_CURSOR_NORMAL)
    {
        Renderer::the().handle_input(window(), delta_time());
    }
}

void Editor::scroll_callback(double, double yoff)
{
    const auto max = [](auto a, auto b) {
        return a > b ? a : b;
    };

    const auto min = [](auto a, auto b) {
        return a < b ? a : b;
    };

    if (cursor_mode() != GLFW_CURSOR_NORMAL) {
        config::the().renderer.move_speed += (((float)yoff) - (yoff < 0.0f ? -0.995f : 0.995f));
        config::the().renderer.move_speed = min(max(config::the().renderer.move_speed, 0.0f), .2f);
    }
}

void Editor::mouse_button_callback(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        Editor::the().set_can_resize(false);
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        Editor::the().set_can_resize(true);
    //TODO: Ctrl+click should select multiple objects for exporting into one mesh / moving and so on
    double mouse_x, mouse_y;
    glfwGetCursorPos(window(), &mouse_x, &mouse_y);
    const auto location = Renderer::the().render_location();
    const auto render_size = Renderer::the().render_size();
    static bool last_left_click_was_inside_renderer{false};
    if (config::the().windows.renderer && last_left_click_was_inside_renderer && button == GLFW_MOUSE_BUTTON_LEFT && cursor_mode() == GLFW_CURSOR_NORMAL && action == GLFW_RELEASE
        && mouse_x > location.x && mouse_y > location.y
        && mouse_x < location.x + render_size.x && mouse_y < location.y + render_size.y)
    {
        Renderer::the().select(mouse_x - location.x, mouse_y - location.y, mods & GLFW_MOD_CONTROL);
    }
    if (config::the().windows.renderer && button == GLFW_MOUSE_BUTTON_LEFT && cursor_mode() == GLFW_CURSOR_NORMAL && action == GLFW_PRESS && mouse_x > location.x && mouse_y > location.y
        && mouse_x < location.x + render_size.x && mouse_y < location.y + render_size.y)
        last_left_click_was_inside_renderer = true;
    else
        last_left_click_was_inside_renderer = false;
}

void Editor::key_callback(int key, int scancode, int action, int mods)
{
    (void)mods;
    (void)scancode;
    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        if (cursor_mode() != GLFW_CURSOR_DISABLED) {
            set_cursor_mode(GLFW_CURSOR_DISABLED);
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            io()->ConfigFlags |= ImGuiConfigFlags_NoMouse;
        }
        else {
            set_cursor_mode(GLFW_CURSOR_NORMAL);
            io()->ConfigFlags = io()->ConfigFlags & ~(1<<4);
        }
    }
}

void Editor::cursor_position_callback(double x, double y)
{
    static double lastX, lastY;
    if (cursor_mode() == GLFW_CURSOR_DISABLED) {
        double x_diff = x - lastX;
        double y_diff = y - lastY;
        // TODO: more abstraction
        Renderer::the().camera().set_yaw_pitch(
                Renderer::the().camera().yaw() + (float)(x_diff * 0.1),
                Renderer::the().camera().pitch() - (float)(y_diff * 0.1)
        );
    }
    lastX = x;
    lastY = y;
}

void Editor::imgui_init()
{
    m_io = &ImGui::GetIO();
    io()->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io()->ConfigDockingWithShift = false;
    //io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.WindowRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.TabRounding = 0.0f;
}
