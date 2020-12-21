
#include <Gui/RenderBuffer.h>
#include <glad/glad.h>
#include <Utility/dbgprint.h>

void TexturedRenderBuffer::resize_buffer(int width, int height) {
	static int msaa = 4;
	if (is_allocated()) {
		glDeleteRenderbuffers(1, &m_depthbuffer);
		glDeleteTextures(1, &m_texture);
		glDeleteFramebuffers(1, &m_fbo);
	}

	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, width, height);

	glGenRenderbuffers(1, &m_depthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthbuffer);

	glViewport(0, 0, width, height);

	const auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (result) {
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:				dbgprint("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:		dbgprint("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:				dbgprint("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n"); break;
	case GL_FRAMEBUFFER_UNSUPPORTED:						dbgprint("GL_FRAMEBUFFER_UNSUPPORTED\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:				dbgprint("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n"); break;
	case GL_RENDERBUFFER_SAMPLES:							dbgprint("GL_RENDERBUFFER_SAMPLES\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR:		dbgprint("GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR\n"); break;
	case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:				dbgprint("GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE\n"); break;
	}

	if (result != GL_FRAMEBUFFER_COMPLETE) {
		dbgprint("Framebuffer error: %d\n", glGetError());
	}

	m_allocated = true;
}
