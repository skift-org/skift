#pragma once

#include <libsystem/Common.h>

template <bool B, class T = void>
struct EnableIf
{
};

template <class T>
struct EnableIf<true, T>
{
    using Type = T;
};

template <class T>
struct AddConst
{
    using Type = const T;
};

template <class T>
struct RemoveConst
{
    using Type = T;
};

template <class T>
struct RemoveConst<const T>
{
    using Type = T;
};

template <class T>
struct RemoveVolatile
{
    using Type = T;
};

template <class T>
struct RemoveVolatile<volatile T>
{
    using Type = T;
};

template <class T>
struct RemoveConstVolatile
{
    typedef typename RemoveVolatile<typename RemoveConst<T>::Type>::Type Type;
};

template <class T, T v>
struct IntegralConstant
{
    static constexpr T value = v;
    using ValueType = T;
    using Type = IntegralConstant;
    constexpr operator ValueType() const { return value; }
    constexpr ValueType operator()() const { return value; }
};

using FalseType = IntegralConstant<bool, false>;

using TrueType = IntegralConstant<bool, true>;

template <class T>
struct IsLvalueReference : FalseType
{
};

template <class T>
struct IsLvalueReference<T &> : TrueType
{
};

template <class T>
struct __IsPointerHelper : FalseType
{
};

template <class T>
struct __IsPointerHelper<T *> : TrueType
{
};

template <class T>
struct IsPointer : __IsPointerHelper<typename RemoveConstVolatile<T>::Type>
{
};

template <class>
struct IsFunction : FalseType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...)> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...)> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) const> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) volatile> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) const volatile> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) const &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) volatile &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) const volatile &> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) &&> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) &&> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const &&> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) const &&> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) volatile &&> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) volatile &&> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args...) const volatile &&> : TrueType
{
};

template <class Ret, class... Args>
struct IsFunction<Ret(Args..., ...) const volatile &&> : TrueType
{
};

template <class T>
struct IsRvalueReference : FalseType
{
};

template <class T>
struct IsRvalueReference<T &&> : TrueType
{
};

template <class T>
struct RemovePointer
{
    using Type = T;
};

template <class T>
struct RemovePointer<T *>
{
    using Type = T;
};

template <class T>
struct RemovePointer<T *const>
{
    using Type = T;
};

template <class T>
struct RemovePointer<T *volatile>
{
    using Type = T;
};

template <class T>
struct RemovePointer<T *const volatile>
{
    using Type = T;
};

template <typename T, typename U>
struct IsSame : FalseType
{
};

template <typename T>
struct IsSame<T, T> : TrueType
{
};

template <bool condition, class TrueType, class FalseType>
struct Conditional
{
    typedef TrueType Type;
};

template <class TrueType, class FalseType>
struct Conditional<false, TrueType, FalseType>
{
    typedef FalseType Type;
};

template <typename T>
struct RemoveReference
{
    using Type = T;
};

template <class T>
struct RemoveReference<T &>
{
    using Type = T;
};

template <class T>
struct RemoveReference<T &&>
{
    using Type = T;
};

template <typename T>
struct MakeUnsigned
{
};

template <>
struct MakeUnsigned<signed char>
{
    typedef unsigned char Type;
};

template <>
struct MakeUnsigned<short>
{
    typedef unsigned short Type;
};

template <>
struct MakeUnsigned<int>
{
    typedef unsigned Type;
};

template <>
struct MakeUnsigned<long>
{
    typedef unsigned long Type;
};

template <>
struct MakeUnsigned<long long>
{
    typedef unsigned long long Type;
};

template <>
struct MakeUnsigned<unsigned char>
{
    typedef unsigned char Type;
};

template <>
struct MakeUnsigned<unsigned short>
{
    typedef unsigned short Type;
};

template <>
struct MakeUnsigned<unsigned int>
{
    typedef unsigned Type;
};

template <>
struct MakeUnsigned<unsigned long>
{
    typedef unsigned long Type;
};

template <>
struct MakeUnsigned<unsigned long long>
{
    typedef unsigned long long Type;
};

template <typename T>
struct MakeSigned
{
};

template <>
struct MakeSigned<signed char>
{
    typedef signed char Type;
};

template <>
struct MakeSigned<short>
{
    typedef short Type;
};

template <>
struct MakeSigned<int>
{
    typedef int Type;
};

template <>
struct MakeSigned<long>
{
    typedef long Type;
};

template <>
struct MakeSigned<long long>
{
    typedef long long Type;
};

template <>
struct MakeSigned<unsigned char>
{
    typedef char Type;
};

template <>
struct MakeSigned<unsigned short>
{
    typedef short Type;
};

template <>
struct MakeSigned<unsigned int>
{
    typedef int Type;
};

template <>
struct MakeSigned<unsigned long>
{
    typedef long Type;
};

template <>
struct MakeSigned<unsigned long long>
{
    typedef long long Type;
};

template <class T>
struct IsVoid : IsSame<void, typename RemoveConstVolatile<T>::Type>
{
};

template <class T>
struct IsConst : FalseType
{
};

template <class T>
struct IsConst<const T> : TrueType
{
};

template <typename T>
struct IsUnion : public IntegralConstant<bool, __is_union(T)>
{
};

template <typename T>
struct IsClass : public IntegralConstant<bool, __is_class(T)>
{
};

template <typename Base, typename Derived>
struct IsBaseOf : public IntegralConstant<bool, __is_base_of(Base, Derived)>
{
};

template <typename T>
struct __IsIntegral : FalseType
{
};

template <>
struct __IsIntegral<uint8_t> : TrueType
{
};

template <>
struct __IsIntegral<uint16_t> : TrueType
{
};

template <>
struct __IsIntegral<uint32_t> : TrueType
{
};

template <>
struct __IsIntegral<uint64_t> : TrueType
{
};

template <typename T>
using IsIntegral = __IsIntegral<typename MakeUnsigned<typename RemoveConstVolatile<T>::Type>::Type>;

template <typename T>
struct __IsFloatingPoint : FalseType
{
};

template <>
struct __IsFloatingPoint<float> : TrueType
{
};

template <>
struct __IsFloatingPoint<double> : TrueType
{
};

template <typename T>
using IsFloatingPoint = __IsFloatingPoint<typename RemoveConstVolatile<T>::Type>;

template <typename ReferenceType, typename T>
using CopyConst =
    typename Conditional<IsConst<ReferenceType>::value, typename AddConst<T>::Type, typename RemoveConst<T>::Type>::Type;

template <typename T, typename... Ts>
concept In = (IsSame<T, Ts>::value || ...);

template <typename... Ts>
struct First {};

template <typename TFirst, typename... Ts>
struct First<TFirst, Ts...>
{
  using type = TFirst;
};
