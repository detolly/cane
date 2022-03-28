#pragma once

#include <stdexcept>

class ez_stream {
public:
	ez_stream(const char* buffer, size_t len) : m_buffer(buffer), m_len(len) {}
	~ez_stream() = default;

	void seek(size_t index) { m_index = index; }
	[[nodiscard]] const inline size_t tell() { return m_index; }

	template <typename T, size_t size = sizeof(T)>
	[[nodiscard]] const T read() {
        if (m_index + size > m_len)
            throw std::out_of_range("ur doing it wrong");
		T ret = *(T*)(m_buffer + m_index);
		m_index += size;
		return ret;
	}

    template <typename T, size_t size = sizeof(T)>
    [[nodiscard]] const T read_at(size_t index) {
        const auto before = tell();
	    seek(index);
	    T ret = read<T, size>();
	    seek(before);
	    return ret;
    }

	const char* buffer() const { return m_buffer; }
	std::size_t size() const { return m_len; }

	template <size_t size = sizeof(glm::vec3)>
	const glm::vec3 read_sly_vec() {
		glm::vec3 ret = read<glm::vec3, size>();
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

