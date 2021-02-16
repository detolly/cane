#pragma once

#include <stdexcept>

class ez_stream {
public:
	ez_stream(const char* buffer, unsigned int len) : m_buffer(buffer), m_len(len) {}
	~ez_stream() = default;

	void seek(unsigned int index) { m_index = index; }
	[[nodiscard]] const inline size_t tell() { return m_index; }

	template <typename T, unsigned int size = sizeof(T)>
	[[nodiscard]] const T read() {
        if (m_index + size > m_len)
            throw std::out_of_range("ur doing it wrong");
		T ret = *(T*)((const char*)m_buffer + m_index);
		m_index += size;
		return ret;
	}

	const char* buffer() const { return m_buffer; }
	const unsigned int size() const { return m_len; }

	template <unsigned int size = sizeof(glm::vec3)>
	const glm::vec3 read_sly_vec() {
		glm::vec3 ret = read<glm::vec3>();
		float temp = ret.y;
		ret.y = ret.z;
		ret.z = temp;
		ret.x = -ret.x;
		ret /= 100.0f;
		return ret;
	}

private:
	const char* m_buffer{ nullptr };
	size_t  m_index{ 0 };
	size_t  m_len{};
};