export module Karm.Core:meta.signess;

namespace Karm::Meta {

// MARK: Signed ----------------------------------------------------------------

template <typename T>
struct _MakeSigned {
    using Type = void;
};

template <>
struct _MakeSigned<char> {
    using Type = signed char;
};

template <>
struct _MakeSigned<unsigned char> {
    using Type = signed char;
};

template <>
struct _MakeSigned<short> {
    using Type = short;
};

template <>
struct _MakeSigned<unsigned short> {
    using Type = short;
};

template <>
struct _MakeSigned<int> {
    using Type = int;
};

template <>
struct _MakeSigned<unsigned int> {
    using Type = int;
};

template <>
struct _MakeSigned<long> {
    using Type = long;
};

template <>
struct _MakeSigned<unsigned long> {
    using Type = long;
};

template <>
struct _MakeSigned<long long> {
    using Type = long long;
};

template <>
struct _MakeSigned<unsigned long long> {
    using Type = long long;
};

export template <typename T>
using MakeSigned = typename _MakeSigned<T>::Type;

// MARK:- Unsigned -------------------------------------------------------------

template <typename T>
struct _MakeUnsigned {
    using Type = void;
};

template <>
struct _MakeUnsigned<char> {
    using Type = unsigned char;
};

template <>
struct _MakeUnsigned<unsigned char> {
    using Type = unsigned char;
};

template <>
struct _MakeUnsigned<short> {
    using Type = unsigned short;
};

template <>
struct _MakeUnsigned<unsigned short> {
    using Type = unsigned short;
};

template <>
struct _MakeUnsigned<int> {
    using Type = unsigned int;
};

template <>
struct _MakeUnsigned<unsigned int> {
    using Type = unsigned int;
};

template <>
struct _MakeUnsigned<long> {
    using Type = unsigned long;
};

template <>
struct _MakeUnsigned<unsigned long> {
    using Type = unsigned long;
};

template <>
struct _MakeUnsigned<long long> {
    using Type = unsigned long long;
};

template <>
struct _MakeUnsigned<unsigned long long> {
    using Type = unsigned long long;
};

export template <typename T>
using MakeUnsigned = typename _MakeUnsigned<T>::Type;

} // namespace Karm::Meta
