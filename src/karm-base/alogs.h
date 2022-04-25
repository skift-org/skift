#pragma once

#include "opt.h"
#include "std.h"

namespace Karm::Base
{

static constexpr bool contains(auto const &container, auto const &value)
{
    for (auto const &i : container)
    {
        if (i == value)
        {
            return true;
        }
    }

    return false;
}

static Opt<size_t> index_of(auto const &container, auto const &value)
{
    size_t index = 0;
    for (auto const &i : container)
    {
        if (i == value)
        {
            return index;
        }
        index++;
    }
    return NONE;
}

static constexpr bool any(auto const &container, auto const &predicate)
{
    for (auto const &i : container)
    {
        if (predicate(i))
        {
            return true;
        }
    }
    return false;
}

static constexpr bool all(auto const &container, auto const &predicate)
{
    for (auto const &i : container)
    {
        if (!predicate(i))
        {
            return false;
        }
    }

    return true;
}

static constexpr bool none(auto const &container, auto const &predicate)
{
    for (auto const &i : container)
    {
        if (predicate(i))
        {
            return false;
        }
    }

    return true;
}

static constexpr auto find(auto const &container, auto const &predicate)
    -> Opt<decltype(*container.first())>
{
    for (auto const &i : container)
    {
        if (predicate(i))
        {
            return i;
        }
    }

    return NONE;
}

static constexpr size_t len(auto const &container)
{
    if constexpr (requires { container.len(); })
    {
        return container.len();
    }
    else
    {
        size_t len = 0;

        for (auto const &i : container)
        {
            len++;
        }

        return len;
    }
}

} // namespace Karm::Base
