#pragma once

#include <Gui/RenderBuffer.h>
#include <Utility/Singleton.h>

class ModelViewer : TexturedRenderBuffer, public Singleton<ModelViewer>
{
public:
	ModelViewer() {}

	void init();

	void render();

	void on_load() { m_selected = -1; }
	void on_close() { m_selected = -1; }

	int selected() const { return m_selected; }
	void set_selected(int value);

private:
	int m_selected{ -1 };
};

