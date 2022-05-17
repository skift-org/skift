#pragma once

#include "_prelude.h"

#include "error.h"
#include "opt.h"
#include "std.h"

namespace Karm {

template <typename Value>
struct [[nodiscard]] Result {
    Error _error;
    Opt<Value> _value = NONE;

    constexpr Result(Error error) : _error(error) {}
    constexpr Result(Value &&value) : _error(), _value(value) {}

    constexpr operator bool() { return _error; }
    constexpr Error none() const { return _error; }
    constexpr Value &unwrap() { return _value.unwrap(); }
    constexpr Value take() { return _value.take(); }
};

static_assert(Tryable<Result<int>>);

} // namespace Karm
