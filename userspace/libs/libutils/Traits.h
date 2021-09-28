#pragma once

#include <libutils/Prelude.h>

namespace Utils
{

template <bool B, typename T = void>
struct EnableIf
{
};

template <typename T>
struct EnableIf<true, T>
{
    using Type = T;
};

template <typename T>
struct AddConst
{
    using Type = const T;
};

template <typename T>
struct RemoveConst
{
    using Type = T;
};

template <typename T>
struct RemoveConst<const T>
{
    using Type = T;
};

template <typename T>
struct RemoveVolatile
{
    using Type = T;
};

template <typename T>
struct RemoveVolatile<volatile T>
{
    using Type = T;
};

template <typename T>
struct RemoveConstVolatile
{
    typedef typename RemoveVolatile<typename RemoveConst<T>::Type>::Type Type;
};

template <typename T, T v>
struct Constant
{
    using ValueType = T;
    using Type = Constant;

    static constexpr T value = v;

    constexpr ALWAYS_INLINE operator ValueType() const { return value; }
    constexpr ALWAYS_INLINE ValueType operator()() const { return value; }
};

using FalseType = Constant<bool, false>;

using TrueType = Constant<bool, true>;

template <typename T>
struct IsLvalueReference : FalseType
{
};

template <typename T>
struct IsLvalueReference<T &> : TrueType
{
};

template <typename T>
struct __IsPointer : FalseType
{
};

template <typename T>
struct __IsPointer<T *> : TrueType
{
};

template <typename T>
struct IsPointer : __IsPointer<typename RemoveConstVolatile<T>::Type>
{
};

template <typename>
struct IsFunction : FalseType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...)> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...)> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) const> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) const> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) volatile> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) volatile> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) const volatile> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) const volatile> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) const &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) const &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) volatile &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) volatile &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) const volatile &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) const volatile &> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) &&> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) &&> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) const &&> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) const &&> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) volatile &&> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) volatile &&> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args...) const volatile &&> : TrueType
{
};

template <typename Ret, typename... Args>
struct IsFunction<Ret(Args..., ...) const volatile &&> : TrueType
{
};

template <typename T>
struct IsRvalueReference : FalseType
{
};

template <typename T>
struct IsRvalueReference<T &&> : TrueType
{
};

template <typename T>
struct RemovePointer
{
    using Type = T;
};

template <typename T>
struct RemovePointer<T *>
{
    using Type = T;
};

template <typename T>
struct RemovePointer<T *const>
{
    using Type = T;
};

template <typename T>
struct RemovePointer<T *volatile>
{
    using Type = T;
};

template <typename T>
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

template <bool condition, typename TrueType, typename FalseType>
struct Conditional
{
    typedef TrueType Type;
};

template <typename TrueType, typename FalseType>
struct Conditional<false, TrueType, FalseType>
{
    typedef FalseType Type;
};

template <typename T>
struct RemoveReference
{
    using Type = T;
};

template <typename T>
struct RemoveReference<T &>
{
    using Type = T;
};

template <typename T>
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

template <typename T>
struct IsVoid : IsSame<void, typename RemoveConstVolatile<T>::Type>
{
};

template <typename T>
struct IsConst : FalseType
{
};

template <typename T>
struct IsConst<const T> : TrueType
{
};

template <typename T>
struct IsConst<const T *> : TrueType
{
};

template <typename T>
struct IsUnion : public Constant<bool, __is_union(T)>
{
};

template <typename T>
struct IsClass : public Constant<bool, __is_class(T)>
{
};

template <typename Base, typename Derived>
struct IsBaseOf : public Constant<bool, __is_base_of(Base, Derived)>
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
using CopyConst = typename Conditional<IsConst<ReferenceType>::value, typename AddConst<T>::Type, typename RemoveConst<T>::Type>::Type;

template <typename T, typename... Ts>
concept In = (IsSame<T, Ts>::value || ...);

template <typename... Ts>
struct First
{
};

template <typename TFirst, typename... Ts>
struct First<TFirst, Ts...>
{
    using Type = TFirst;
};

template <typename TQuery, typename... Ts>
struct IndexOf;

template <typename TQuery>
struct IndexOf<TQuery> : Constant<int, -1>
{
};

template <typename TQuery, typename TFirst, typename... Ts>
struct IndexOf<TQuery, TFirst, Ts...> : Constant<int, IsSame<TQuery, TFirst>::value ? 0 : IndexOf<TQuery, Ts...>::value + 1>
{
};

template <size_t arg1, size_t... others>
struct Max;

template <size_t arg>
struct Max<arg>
{
    static constexpr size_t value = arg;
};

template <size_t arg1, size_t arg2, size_t... others>
struct Max<arg1, arg2, others...>
{
    static constexpr size_t value = arg1 >= arg2 ? Max<arg1, others...>::value : Max<arg2, others...>::value;
};

} // namespace Utils
