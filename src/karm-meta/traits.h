#pragma once

namespace Karm::Meta {

template <typename T>
concept Integral = __is_integral(T);

template <typename T>
concept Signed = __is_signed(T);

template <typename T>
concept Unsigned = __is_unsigned(T);

template <typename T>
concept SignedIntegral = Integral<T> && Signed<T>;

template <typename T>
concept UnsignedIntegral = Integral<T> && Unsigned<T>;

template <typename T>
concept Trivial = __is_trivial(T);

template <typename Derived, typename Base>
concept Derive = __is_base_of(Base, Derived);

template <typename T, typename U>
inline constexpr bool _Same = false;

template <typename T>
inline constexpr bool _Same<T, T> = true;

template <typename T, typename U>
concept Same = _Same<T, U>;

} // namespace Karm::Meta
