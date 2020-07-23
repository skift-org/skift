#pragma once

template <typename T>
T &&move(T &arg)
{
    return static_cast<T &&>(arg);
}

template <typename T>
void swap(T &left, T &right)
{
    T tmp = move(left);
    left = move(right);
    right = move(tmp);
}

template <typename T>
struct Identity
{
    typedef T Type;
};

template <class T>
constexpr T &&forward(typename Identity<T>::Type &param)
{
    return static_cast<T &&>(param);
}
