#pragma once

#include <iostream>

template <typename... T>
inline static void dbgprint(const char* a, T ...args) {
#ifndef NDEBUG
	printf(a, args...);
#endif
}

inline static void dbgprintf(const char* a) {
#ifndef NDEBUG
	printf(a);
#endif
}