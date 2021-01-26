#include "Editor.h"
#include <Gui/DebugInformation.h>
#include <Gui/Renderer.h>
#include <Gui/ModelBrowser.h>
#include <Gui/ModelViewer.h>
#include <Gui/RendererOptions.h>
#include <Structs/SlyLevelFile.h>

void Editor::init()
{
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

void Editor::size_callback(int width, int height)
{
	Renderer::the().set_should_recalculate_projection(true);
}

Editor Editor::g_editor;