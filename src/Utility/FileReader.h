#pragma once

#include <fstream>
#include <filesystem>
#include <cstring>

class FileReader {
public:
	explicit FileReader(const char* loc, bool should_give_ownership = false) : m_loc(loc), m_should_give_ownership(should_give_ownership) {};
	~FileReader() { if (!m_should_give_ownership) delete[] m_buf; }

	const char* read() {
		const auto path = std::filesystem::current_path();
		std::ifstream file = std::ifstream(path / m_loc, std::ios::in | std::ios::binary | std::ios::ate);
		if (file.is_open()) {
		    m_was_opened = true;
			size_t len = static_cast<size_t>(file.tellg());
			m_len = len;
			char* buf = new char[len];
			m_buf = buf;
			memset(buf, 0, m_len);
			file.seekg(0, std::ios::beg);
			file.read(buf, len);
			file.close();
			return buf;
		}
		return nullptr;
	};

	bool was_opened() const { return m_was_opened; }
	size_t length() const { return m_len; }

private:
    bool m_was_opened{false};
	char* m_buf{nullptr};
	const char* m_loc{nullptr};
	size_t m_len{0};
	bool m_should_give_ownership;
};