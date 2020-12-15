#pragma once

class ez_stream {
public:
	ez_stream(const char* buffer, size_t len) : m_buffer(buffer), m_len(len) {}

	void seek(int index) { m_index = index; }
	[[nodiscard]] const inline unsigned int tell() { return m_index; }

	template <typename T, size_t size = sizeof(T)> const inline T read() {
		T ret = *(T*)((uint32_t)m_buffer + m_index);
		m_index += size;
		if (m_index > m_len)
			throw std::exception("ur doing it wrong");
		return ret;
	}

private:
	const char* m_buffer{ nullptr };
	unsigned int m_index{ 0 };
	size_t m_len{};
};