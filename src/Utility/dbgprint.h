#pragma once

#include <cstdio>

namespace {
template<typename ...T> void sink(const T& ...) {}
}

inline static void dbgprint(const char* a) {
#ifndef NDEBUG
    printf("%s", a);
#endif
#ifdef NDEBUG
    (void)(a);
#endif
}

template <typename... T>
inline static void dbgprint(const char* a, const T& ...args) {
#ifndef NDEBUG
    printf(a, args...);
#endif
#ifdef NDEBUG
    (void)(a);
    sink(args...);
#endif
}

