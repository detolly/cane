#pragma once

class ez_stream {
public:
	ez_stream(const char* buffer, unsigned int len) : m_buffer(buffer), m_len(len) {}
	~ez_stream() = default;

	void seek(unsigned int index) { m_index = index; }
	[[nodiscard]] const inline unsigned int tell() { return m_index; }

	template <typename T, unsigned int size = sizeof(T)>
	[[nodiscard]] const T read() {
		T ret = *(T*)((const char*)m_buffer + m_index);
		m_index += size;
		if (m_index > m_len)
			throw std::exception("ur doing it wrong");
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
	unsigned int m_index{ 0 };
	unsigned int  m_len{};
};