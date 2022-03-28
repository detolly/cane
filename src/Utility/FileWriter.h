//
// Created by Thomas on 2/9/2021.
//

#ifndef CANE_FILEWRITER_H
#define CANE_FILEWRITER_H

#include <fstream>
#include <filesystem>
#include <cstring>

class FileWriter {
public:
    FileWriter(const char* loc) : m_loc(loc) {}
    ~FileWriter() = default;

    template<typename T>
    void write(const T* ptr, size_t size) {
        //rewrite this also
        const auto path = std::filesystem::current_path();
        std::ofstream file = std::ofstream(path / m_loc, std::ios::out | std::ios::binary | std::ios::beg);
        if (file.is_open()) {
            m_was_opened = true;
            file.write((const char*)ptr, size);
            file.close();
        }
        //TODO: exception handling?
    };

    bool was_opened() const { return m_was_opened; }

private:
    bool m_was_opened{false};
    const char* m_loc{nullptr};
};

#endif // CANE_FILEWRITER_H

