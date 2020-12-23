#include "Editor.h"
#include <Gui/DebugInformation.h>
#include <Gui/RendererWindow.h>
#include <Gui/ModelBrowser.h>
#include <Gui/ModelViewer.h>
#include <Structs/SlyLevelFile.h>

void Editor::init()
{
	Editor::the().m_renderer_window = new RendererWindow();
	Editor::the().m_debug_window = new DebugInformation();
	Editor::the().m_model_browser = new ModelBrowser();
	Editor::the().m_model_viewer = new ModelViewer();
}

void Editor::open(const char* file)
{
	if (has_file_loaded())
		delete m_level;
	m_level = new SlyLevelFile(file);
	debug_window()->on_load();
	renderer()->on_load();
	model_browser()->on_load();
	model_viewer()->on_load();
}

void Editor::close()
{
	if (has_file_loaded())
		delete m_level;
	m_level = nullptr;
	debug_window()->on_close();
	renderer()->on_close();
	model_browser()->on_close();
	model_viewer()->on_close();
}

void Editor::size_callback(int width, int height)
{
	renderer()->set_should_recalculate_projection(true);
}

Editor Editor::g_editor;