#pragma once

#include <glm/glm.hpp>

typedef unsigned int GLuint;

class TexturedRenderBuffer 
{
public:
    explicit TexturedRenderBuffer() = default;
    ~TexturedRenderBuffer() {
        free_buffer();
    }

    TexturedRenderBuffer(const TexturedRenderBuffer&) = delete;
    TexturedRenderBuffer(TexturedRenderBuffer&& buffer) noexcept
    {
        *this = std::move(buffer);
    }
    TexturedRenderBuffer& operator=(TexturedRenderBuffer&& buffer)
    {
        m_fbo = buffer.m_fbo;
        m_depthbuffer = buffer.m_depthbuffer;
        m_texture = buffer.m_texture;
        if (buffer.m_allocated) {
            buffer.m_allocated = false;
            m_allocated = true;
        }
        return *this;
    }

    void free_buffer();
    void resize_buffer(int width, int height);
    void resize_buffer(float width, float height);

    bool is_allocated() const { return m_allocated; }
    glm::u32 fbo() const { return m_fbo; }
    glm::u32 depth_buffer() const { return m_depthbuffer; }
    glm::u32 texture() const { return m_texture; }

private:
    glm::u32 m_fbo{ 0 }, m_depthbuffer{ 0 }, m_texture{ 0 };
    bool m_allocated{false};
};

