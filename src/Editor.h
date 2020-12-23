#pragma once

class SlyLevelFile;
class RendererWindow;
class DebugInformation;
class ModelBrowser;
class ModelViewer;

class Editor {
private:
	Editor() {}
public:
	static Editor& the() { return g_editor; };
	static void init();


	inline RendererWindow* renderer() { return m_renderer_window; }
	inline DebugInformation* debug_window() { return m_debug_window;  }
	inline ModelBrowser* model_browser() { return m_model_browser; }
	inline ModelViewer* model_viewer() { return m_model_viewer; }

	SlyLevelFile* level_file() const { return m_level; }

	void set_can_resize(bool value) { m_can_resize = value; }
	const bool can_resize() const { return m_can_resize; }

	void open(const char* file);
	void close();
	bool has_file_loaded() { return m_level != nullptr; }

	void size_callback(int width, int height);

private:
	static Editor g_editor;
	SlyLevelFile* m_level{nullptr};

	RendererWindow* m_renderer_window;
	DebugInformation* m_debug_window;
	ModelBrowser* m_model_browser;
	ModelViewer* m_model_viewer;

	bool m_can_resize { true };

};