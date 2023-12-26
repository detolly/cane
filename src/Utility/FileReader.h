#pragma once

#include <fstream>
#include <filesystem>
#include <cstring>

class FileReader {
public:
	explicit FileReader(std::string_view loc) : m_location(loc) {}

	std::string read();

	bool was_opened() const { return m_was_opened; }
	size_t length() const { return m_len; }

private:
    bool m_was_opened{ false };
	size_t m_len{ 0 };
    
	std::string m_location;
};

