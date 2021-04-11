#pragma once

#define __enum_flags(__type)                                                                  \
    inline __type operator~(__type a) { return (__type) ~(int)a; }                            \
    inline __type operator|(__type a, __type b) { return (__type)((int)a | (int)b); }         \
    inline __type operator&(__type a, __type b) { return (__type)((int)a & (int)b); }         \
    inline __type operator^(__type a, __type b) { return (__type)((int)a ^ (int)b); }         \
    inline __type &operator|=(__type &a, __type b) { return (__type &)((int &)a |= (int)b); } \
    inline __type &operator&=(__type &a, __type b) { return (__type &)((int &)a &= (int)b); } \
    inline __type &operator^=(__type &a, __type b) { return (__type &)((int &)a ^= (int)b); }
