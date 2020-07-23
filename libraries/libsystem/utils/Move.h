#pragma once

template <typename T>
struct Identity
{
    using Type = T;
};

template <typename T>
struct Identity<T &>
{
    using Type = T;
};

template <typename T>
struct Identity<T &&>
{
    using Type = T;
};

template <typename T>
constexpr typename Identity<T>::Type &&move(T &&arg)
{
    return static_cast<typename Identity<T>::Type &&>(arg);
}

template <class T>
constexpr T &&forward(typename Identity<T>::Type &param)
{
    return static_cast<T &&>(param);
}

template <class T>
constexpr T &&forward(typename Identity<T>::Type &&param)
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
