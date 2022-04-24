#pragma once

#include <karm-base/error.h>
#include <karm-base/opt.h>

namespace Karm::Base
{

template <typename Value>
struct Result
{
    Error _error;
    Opt<Value> _value = NONE;

    constexpr Result(Error error) : _error(error) {}
    constexpr Result(Value value) : _error(), _value(value) {}

    constexpr operator bool() { return _error; }
    constexpr Error error() const { return _error; }
    constexpr Value unwrap() { return _value.unwrap(); }
};

} // namespace Karm::Base
