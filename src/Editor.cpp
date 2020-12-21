#include "Editor.h"

void Editor::open(const char* file)
{
	if (has_file_loaded())
		delete m_level;
	m_level = new SlyLevelFile(file);
}

void Editor::size_callback(int width, int height)
{
	renderer().set_should_recalculate_projection(true);
}

Editor Editor::g_editor;