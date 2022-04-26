#pragma once

#include "_prelude.h"

#include "opt.h"
#include "std.h"

namespace Karm::Base {

static Opt<size_t> index_of(auto const &container, auto const &value) {
    size_t index = 0;
    for (auto const &i : container) {
        if (i == value) {
            return index;
        }
        index++;
    }
    return NONE;
}

static constexpr bool any(auto const &container, auto const &predicate) {
    for (auto const &i : container) {
        if (predicate(i)) {
            return true;
        }
    }
    return false;
}

static constexpr bool all(auto const &container, auto const &predicate) {
    for (auto const &i : container) {
        if (!predicate(i)) {
            return false;
        }
    }

    return true;
}

static constexpr bool none(auto const &container, auto const &predicate) {
    for (auto const &i : container) {
        if (predicate(i)) {
            return false;
        }
    }

    return true;
}

static constexpr auto find(auto const &container, auto const &predicate)
    -> Opt<decltype(*container.first())> {
    for (auto const &i : container) {
        if (predicate(i)) {
            return i;
        }
    }

    return NONE;
}

static constexpr size_t len(auto const &container) {
    if constexpr (requires { container.len(); }) {
        return container.len();
    } else {
        size_t len = 0;

        for (auto const &i : container) {
            len++;
        }

        return len;
    }
}

static constexpr auto map(auto const &container, auto const &mapper)
    -> decltype(container) {
    decltype(container) result;

    for (auto const &i : container) {
        result.push(mapper(i));
    }

    return result;
}

static constexpr auto filter(auto const &container, auto const &predicate) -> decltype(container) {
    decltype(container) result;

    for (auto const &i : container) {
        if (predicate(i)) {
            result.push(i);
        }
    }

    return result;
}

static constexpr auto apply(auto const &container, auto const &applier) -> decltype(container) {
    decltype(container) result;

    for (auto const &i : container) {
        applier(i);
    }

    return result;
}

static constexpr auto reduce(auto const &container, auto const &reducer, auto const &initial)
    -> decltype(initial) {
    auto result = initial;

    for (auto const &i : container) {
        result = reducer(result, i);
    }

    return result;
}

} // namespace Karm::Base
