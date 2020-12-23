#pragma once

#include <Gui/RenderBuffer.h>

class ModelViewer : TexturedRenderBuffer
{
public:
	ModelViewer();

	void render();
	void on_load() { m_selected = -1; }
	void on_close() { m_selected = -1; }

	const int selected() const { return m_selected; }
	void set_selected(int value);

private:
	int m_selected{ -1 };
};