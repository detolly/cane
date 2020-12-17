#pragma once

class ez_stream {
public:
	ez_stream(const char* buffer, unsigned int len) : m_buffer(buffer), m_len(len) {}

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

private:
	const char* m_buffer{ nullptr };
	unsigned int m_index{ 0 };
	unsigned int  m_len{};
};