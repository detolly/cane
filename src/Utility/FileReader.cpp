
#include <Utility/FileReader.h>
#include <Utility/dbgprint.h>

std::string FileReader::read()
{
    std::ifstream file = std::ifstream(m_location, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        m_was_opened = true;
        m_len = static_cast<size_t>(file.tellg());

        std::string contents;
        contents.resize(m_len);

        file.seekg(0, std::ios::beg);
        file.read(contents.data(), m_len);
        file.close();

        return contents;
    }
    return {};
}