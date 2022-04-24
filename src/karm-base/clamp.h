#pragma once

namespace Karm::Base
{

constexpr auto max(auto value) { return value; }
constexpr auto max(auto first, auto... rest)
{
    auto rhs = max(rest...);
    return first > rhs ? first : rhs;
}

constexpr auto min(auto value) { return value; }
constexpr auto min(auto first, auto... rest)
{
    auto rhs = min(rest...);
    return first < rhs ? first : rhs;
}

} // namespace Karm::Base
