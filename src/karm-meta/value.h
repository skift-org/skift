#pragma once

namespace Karm::Meta
{

template <typename T, T value>
struct Value
{
    static constexpr T VALUE = value;
    consteval operator T() const { return value; }
    consteval T operator()() const { return value; }
};

using True = Value<bool, true>;

using False = Value<bool, false>;

} // namespace Karm::Meta
