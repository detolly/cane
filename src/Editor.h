#pragma once

#include <Structs/SlyLevelFile.h>
#include <Gui/DebugInformation.h>
#include <Gui/RendererWindow.h>
#include <Gui/ModelBrowser.h>


class Editor {
private:
	Editor() {}
public:

	RendererWindow* renderer() { return m_renderer_window; }
	DebugInformation* debug_window() { return m_debug_window;  }
	ModelBrowser* model_browser() { return m_model_browser; }

	SlyLevelFile * level_file() const { return m_level; }

	static void init();
	static Editor& the() { return g_editor; };

	void set_can_resize(bool value) { m_can_resize = value; }
	const bool can_resize() const { return m_can_resize; }

	void open(const char* file);
	bool has_file_loaded() { return m_level != nullptr; }

	void size_callback(int width, int height);

private:
	static Editor g_editor;
	SlyLevelFile* m_level{nullptr};

	RendererWindow* m_renderer_window;
	DebugInformation* m_debug_window;
	ModelBrowser* m_model_browser;

	bool m_can_resize { true };

};