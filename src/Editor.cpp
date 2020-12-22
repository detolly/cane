#include "Editor.h"
#include <Gui/DebugInformation.h>
#include <Gui/RendererWindow.h>
#include <Gui/ModelBrowser.h>

void Editor::init()
{
	Editor::the().m_renderer_window = new RendererWindow();
	Editor::the().m_debug_window = new DebugInformation();
	Editor::the().m_model_browser = new ModelBrowser();
}

void Editor::open(const char* file)
{
	if (has_file_loaded())
		delete m_level;
	m_level = new SlyLevelFile(file);
}

void Editor::size_callback(int width, int height)
{
	renderer()->set_should_recalculate_projection(true);
}

Editor Editor::g_editor;