#pragma once

namespace Karm::Meta {

/* --- Signed --------------------------------------------------------------- */

template <typename T>
struct _Signed {
    using Type = void;
};

template <>
struct _Signed<char> {
    using Type = signed char;
};

template <>
struct _Signed<unsigned char> {
    using Type = signed char;
};

template <>
struct _Signed<short> {
    using Type = short;
};

template <>
struct _Signed<unsigned short> {
    using Type = short;
};

template <>
struct _Signed<int> {
    using Type = int;
};

template <>
struct _Signed<unsigned int> {
    using Type = int;
};

template <>
struct _Signed<long> {
    using Type = long;
};

template <>
struct _Signed<unsigned long> {
    using Type = long;
};

template <>
struct _Signed<long long> {
    using Type = long long;
};

template <>
struct _Signed<unsigned long long> {
    using Type = long long;
};

template <typename T>
using Signed = typename _Signed<T>::Type;

/* ---- Unsigned ------------------------------------------------------------ */

template <typename T>
struct _Unsigned {
    using Type = void;
};

template <>
struct _Unsigned<char> {
    using Type = unsigned char;
};

template <>
struct _Unsigned<unsigned char> {
    using Type = unsigned char;
};

template <>
struct _Unsigned<short> {
    using Type = unsigned short;
};

template <>
struct _Unsigned<unsigned short> {
    using Type = unsigned short;
};

template <>
struct _Unsigned<int> {
    using Type = unsigned int;
};

template <>
struct _Unsigned<unsigned int> {
    using Type = unsigned int;
};

template <>
struct _Unsigned<long> {
    using Type = unsigned long;
};

template <>
struct _Unsigned<unsigned long> {
    using Type = unsigned long;
};

template <>
struct _Unsigned<long long> {
    using Type = unsigned long long;
};

template <>
struct _Unsigned<unsigned long long> {
    using Type = unsigned long long;
};

template <typename T>
using Unsigned = typename _Unsigned<T>::Type;

}; // namespace Karm::Meta
