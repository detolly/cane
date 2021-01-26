#pragma once

#include <fstream>
#include <filesystem>
#include <cstring>

class FileReader {
public:
	FileReader(const char* loc, bool should_give_ownership = false) : m_loc(loc), m_buf(nullptr), m_should_give_ownership(should_give_ownership) {};
	~FileReader() { if (!m_should_give_ownership) delete[] m_buf; }

	const char* read() {
		const auto path = std::filesystem::current_path();
		std::ifstream file = std::ifstream(path / m_loc, std::ios::in | std::ios::binary | std::ios::ate);
		if (file.is_open()) {
			std::streamoff len = file.tellg();
			m_len = (size_t)len;
			char* buf = new char[len];
			m_buf = buf;
			memset(buf, 0, len);
			file.seekg(0, std::ios::beg);
			file.read(buf, len);
			file.close();
			return buf;
		}
		return nullptr;
	};

	size_t length() { return m_len; }

private:
	char* m_buf;
	const char* m_loc;
	size_t m_len{0};
	bool m_should_give_ownership;
};