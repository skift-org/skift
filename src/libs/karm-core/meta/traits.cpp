module;

#include <karm-core/macros.h>

export module Karm.Core:meta.traits;

import :meta.decl;

namespace Karm::Meta {

// MARK: Primary type categories -----------------------------------------------

export template <typename T>
concept Void = __is_void(T);

template <typename T>
inline constexpr bool _Nullptr = false;

template <>
inline constexpr bool _Nullptr<decltype(nullptr)> = true;

export template <typename T>
concept Nullptr = _Nullptr<T>;

template <typename T>
inline constexpr bool _Boolean = false;

template <>
inline constexpr bool _Boolean<bool> = true;

export template <typename T>
concept Boolean = _Boolean<T>;

export template <typename T>
concept Float = __is_floating_point(T);

export template <typename T>
concept Array = __is_array(T);

export template <typename T>
concept Enum = __is_enum(T);

export template <typename T>
concept Union = __is_union(T);

export template <typename T>
concept Struct = __is_class(T);

export template <typename T>
concept Integral = __is_integral(T);

// MARK: Type properties -------------------------------------------------------

template <typename T>
struct _Empty {
    char c;
    [[no_unique_address]] T t;
};

export template <typename T>
concept Empty = sizeof(_Empty<T>) == 1;

export template <typename T>
constexpr auto zeroableSizeOf() {
    if constexpr (Meta::Empty<T>)
        return 0uz;
    return sizeof(T);
}

export template <typename T>
concept Trivial = __is_trivial(T);

export template <typename T>
concept StandardLayout = __is_standard_layout(T);

export template <typename T>
concept Pod = Trivial<T> and StandardLayout<T>;

export template <typename T>
concept TrivialyCopyable = __is_trivially_copyable(T);

export template <typename T>
concept Aggregate = __is_aggregate(T);

export template <typename T>
concept Signed = __is_signed(T);

export template <typename T>
concept Unsigned = __is_unsigned(T);

export template <typename T>
concept SignedIntegral = Integral<T> and Signed<T>;

export template <typename T>
concept UnsignedIntegral = Integral<T> and Unsigned<T>;

export template <typename T>
using UnderlyingType = __underlying_type(T);

// MARK: Type relationships ----------------------------------------------------

export template <typename Derived, typename Base>
concept Derive = __is_base_of(Base, Derived);

export template <typename T, typename... Args>
concept Constructible = requires {
    ::new T(declval<Args>()...);
};

export template <typename T>
concept DefaultConstructible = Constructible<T>;

export template <typename T, typename U = T>
concept CopyConstructible = Constructible<T, U const&>;

export template <typename T, typename U = T>
concept MoveConstructible = Constructible<T, U&&>;

export template <typename From, typename To>
concept Convertible = requires {
    declval<void (*)(To)>()(declval<From>());
};

template <typename T, typename U>
inline constexpr bool _Same = false;

template <typename T>
inline constexpr bool _Same<T, T> = true;

export template <typename T, typename U>
concept Same = _Same<T, U>;

/// A type is comparable if it can be compared using the <=> operator.
/// Comparable does not imply Equatable.
export template <typename T, typename U = T>
concept Comparable = requires(T const& a, U const& b) {
    { a <=> b } -> Same<decltype(a <=> b)>;
};

/// A type is equatable if it can be compared for equality.
export template <typename T, typename U = T>
concept Equatable = requires(T const& a, U const& b) {
    { a == b } -> Same<decltype(a == b)>;
};

} // namespace Karm::Meta
