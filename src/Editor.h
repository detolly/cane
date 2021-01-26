#pragma once

class SlyLevelFile;
class Renderer;
class DebugInformation;
class ModelBrowser;
class ModelViewer;
class RendererOptions;

class Editor {
private:
	Editor() = default;
	~Editor() = default;
public:
	static Editor& the() { return g_editor; };
	static void init();

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

	bool m_can_resize { true };

};