#pragma once

#include <cstring>
#include <cstdint>

namespace det {
	static inline bool memcmp(const char* buffer, const char* bMask, const char* szMask, int* advance_by) {
		for (size_t bCount = 0; bCount < std::strlen(szMask); ++bCount) {
			//printf("Read byte: 0x%x,  Byte Mask: 0x%x\r\n", *(BYTE*)(bData + bCount), (BYTE)bMask[bCount]);
			if (szMask[bCount] == 'x' && buffer[bCount] != bMask[bCount]) {
				*advance_by = bCount;
				return false;
			}
		}
		return true;
	}

	static inline int sigscan(const char* buffer, int start, int size, const char* sig, const char* mask, size_t offset)
	{
		for (int i = 0; i < size; i++) {
			int advance_count = 0;
			if (det::memcmp(buffer + start + i, sig, mask, &advance_count)) {
				return start + i + offset;
			}
			i += advance_count;
		}
		return false;
	}
}