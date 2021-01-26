#pragma once

typedef unsigned int GLuint;

class TexturedRenderBuffer {
public:
	TexturedRenderBuffer() {}
	virtual ~TexturedRenderBuffer() {
		free_buffer();
	}

	TexturedRenderBuffer(const TexturedRenderBuffer&) = delete;
	TexturedRenderBuffer(TexturedRenderBuffer&& buffer) noexcept :
		m_fbo(buffer.m_fbo), m_depthbuffer(buffer.m_depthbuffer), m_texture(buffer.m_texture)
	{
		if (buffer.m_allocated) {
			buffer.m_allocated = false;
			m_allocated = true;
		}
	}

	void free_buffer();
	void resize_buffer(int width, int height);
	const bool is_allocated() const { return m_allocated; }
	GLuint fbo() const { return m_fbo; }
	GLuint depth_buffer() const { return m_depthbuffer; }
	GLuint texture() const { return m_texture; }

private:
	GLuint m_fbo{ 0 }, m_depthbuffer{ 0 }, m_texture{ 0 };
	bool m_allocated{false};
};