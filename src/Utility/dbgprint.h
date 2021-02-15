#pragma once

#include <iostream>

template <typename... T>
inline static void dbgprint(const char* a, T ...args) {
#ifndef NDEBUG
	printf(a, args...);
#endif
#ifdef NDEBUG
    (void)(a);
#endif
}

inline static void dbgprintf(const char* a) {
#ifndef NDEBUG
	printf("%s", a);
#endif
#ifdef NDEBUG
    (void)(a);
#endif
}