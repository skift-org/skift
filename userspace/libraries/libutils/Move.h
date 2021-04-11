#pragma once

#include <libutils/Traits.h>

using nullptr_t = decltype(nullptr);

template <typename T>
constexpr typename RemoveReference<T>::Type &&move(T &&arg)
{
    return static_cast<typename RemoveReference<T>::Type &&>(arg);
}

template <class T>
constexpr T &&forward(typename RemoveReference<T>::Type &param)
{
    return static_cast<T &&>(param);
}

template <class T>
constexpr T &&forward(typename RemoveReference<T>::Type &&param)
{
    return static_cast<T &&>(param);
}

template <typename T>
void swap(T &left, T &right)
{
    T tmp = move(left);
    left = move(right);
    right = move(tmp);
}

template <typename T, typename U = T>
constexpr T exchange_and_return_initial_value(T &slot, U &&value)
{
    T old = move(slot);
    slot = forward<U>(value);
    return old;
}
