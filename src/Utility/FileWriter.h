#pragma once

#include <fstream>
#include <filesystem>
#include <cstring>

class FileWriter {
public:
    FileWriter(std::string_view location) : m_location(location) {}
    ~FileWriter() = default;

    template<typename T>
    void write(const T* ptr, size_t size) {
        //rewrite this also
        std::ofstream file = std::ofstream(m_location, std::ios::binary);
        if (file.is_open()) {
            m_was_opened = true;
            file.write(reinterpret_cast<const char*>(ptr), size);
            file.close();
        }
    };

    bool was_opened() const { return m_was_opened; }

private:
    bool m_was_opened{ false };
    std::string m_location;
};
