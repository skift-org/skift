#pragma once

namespace Math
{

template <typename T>
static constexpr T lerp(T from, T to, double transition)
{
    return from + (to - from) * transition;
}

} // namespace Math
