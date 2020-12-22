#pragma once

#include <Gui/RenderBuffer.h>

class ModelViewer : TexturedRenderBuffer
{
public:
	ModelViewer();

	void render();

	const int selected() const { return m_selected; }
	void set_selected(int value);

private:
	int m_selected{ -1 };
};