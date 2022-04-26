#pragma once

#include "_prelude.h"

#include "error.h"
#include "opt.h"

namespace Karm::Base {

template <typename Value>
struct [[nodiscard]] Result {
    Error _error;
    Opt<Value> _value = NONE;

    constexpr Result(Error error) : _error(error) {}
    constexpr Result(Value value) : _error(), _value(value) {}

    constexpr operator bool() { return _error; }
    constexpr Error error() const { return _error; }
    constexpr Value unwrap() { return _value.unwrap(); }
};

} // namespace Karm::Base
