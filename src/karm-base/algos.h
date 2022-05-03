#pragma once

#include "_prelude.h"

#include "opt.h"
#include "std.h"

namespace Karm::Base {

static auto index_of(auto const &c, auto const &value) -> Opt<size_t> {
    size_t index = 0;
    for (auto const &i : c) {
        if (i == value) {
            return index;
        }
        index++;
    }
    return NONE;
}

static auto any(auto const &c, auto const &predicate) -> bool {
    for (auto const &i : c) {
        if (predicate(i)) {
            return true;
        }
    }
    return false;
}

static auto any(auto const &c) -> bool {
    for (auto const &i : c) {
        return true;
    }
    return false;
}

static auto all(auto const &c, auto const &predicate) -> bool {
    for (auto const &i : c) {
        if (!predicate(i)) {
            return false;
        }
    }

    return true;
}

static auto none(auto const &c, auto const &predicate) -> bool {
    for (auto const &i : c) {
        if (predicate(i)) {
            return false;
        }
    }

    return true;
}

static auto find(auto const &c, auto const &predicate) -> Opt<decltype(*c.first())> {
    for (auto const &i : c) {
        if (predicate(i)) {
            return i;
        }
    }

    return NONE;
}

static auto len(auto const &c) -> size_t {
    if (requires { c.len(); }) {
        return c.len();
    } else {
        size_t len = 0;

        for (auto const &i : c) {
            len++;
        }

        return len;
    }
}

static auto map(auto const &c, auto const &mapper) -> decltype(c) {
    decltype(c) result;

    for (auto const &i : c) {
        result.push(mapper(i));
    }

    return result;
}

static auto filter(auto const &c, auto const &predicate) -> decltype(c) {
    decltype(c) result;

    for (auto const &i : c) {
        if (predicate(i)) {
            result.push(i);
        }
    }

    return result;
}

static auto apply(auto const &c, auto const &applier) -> decltype(c) {
    decltype(c) result;

    for (auto const &i : c) {
        applier(i);
    }

    return result;
}

static auto reduce(auto const &c, auto const &reducer, auto const &initial) -> decltype(initial) {
    auto result = initial;

    for (auto const &i : c) {
        result = reducer(result, i);
    }

    return result;
}

} // namespace Karm::Base
