#pragma once

#include <cstring>
#include <string_view>

static inline bool compare(const char* buffer, const std::string_view mask, std::string_view ignored_bytes, int& advance_by) {
    for (size_t bCount = 0; bCount < ignored_bytes.size(); ++bCount) {
        //printf("Read byte: 0x%x,  Byte Mask: 0x%x\r\n", *(BYTE*)(bData + bCount), (BYTE)bMask[bCount]);
        if (ignored_bytes[bCount] == 'x' && buffer[bCount] != mask[bCount]) {
            advance_by = bCount;
            return false;
        }
    }
    return true;
}

static inline int sigscan(const char* buffer, int start, int size, const std::string_view sig, const std::string_view mask, int offset)
{
    for (int i = 0; i < size; i++) {
        int advance_count = 0;
        if (compare(buffer + start + i, sig, mask, advance_count)) {
            return start + i + offset;
        }
        i += advance_count;
    }
    return -1;
}
