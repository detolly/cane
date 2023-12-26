#pragma once

#include <stdexcept>

#include <glm/glm.hpp>

class ez_stream {
public:
	ez_stream(const char* buffer, size_t len) : m_buffer(buffer), m_len(len) {}
	~ez_stream() = default;

	void seek(size_t index) { m_index = index; }
	[[nodiscard]] inline size_t tell() { return m_index; }

	template <typename T, size_t size = sizeof(T)>
	[[nodiscard]] inline T read() {
        if (m_index + size > m_len)
            throw std::out_of_range("ur doing it wrong");
		T ret = *(T*)(m_buffer + m_index);
		m_index += size;
		return ret;
	}

    template <typename T>
    [[nodiscard]] const inline T read_at(size_t index) {
        const auto before = tell();
        seek(index);
	    T ret = read<T>();
        seek(before);
	    return ret;
    }


	template <size_t size = sizeof(glm::vec3)>
	const inline glm::vec3 read_sly_vec() {
		glm::vec3 ret = read<glm::vec3, size>();
		//float temp = ret.y;
		//ret.y = ret.z;
		//ret.z = temp;
		//ret.x = -ret.x;
		//ret /= 100.0f;
		return ret;
	}

    static inline constexpr glm::vec3 to_sly_vector(const glm::vec3 vector)
    {
        //glm::vec3 ret;
        //ret.y = vector.z;
        //ret.z = vector.y;
        //ret.x = -vector.x;
        //ret /= 100.0f;
        return vector;
    }

    inline bool find(const std::string_view lf) {
        std::size_t already_found = 0;
        for (size_t current_index = tell(); current_index < size(); current_index++) {
            if (read_at<char>(current_index) == lf[already_found])
                already_found++;
            else
                already_found = 0;
            if (already_found == lf.size()) {
                seek(current_index - lf.size());
                return true;
            }
        }
        return false;
    }

public:

	const char* buffer() const { return m_buffer; }
	std::size_t size() const { return m_len; }

private:
	const char* m_buffer{ nullptr };
	size_t  m_index{ 0 };
	size_t  m_len{};
};

