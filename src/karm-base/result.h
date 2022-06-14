#pragma once

#include "_prelude.h"

#include "error.h"
#include "opt.h"
#include "std.h"

namespace Karm {

template <typename Value>
struct [[nodiscard]] Result {
    Error _error = OK;
    Opt<Value> _value{};

    constexpr Result(Error::Code code) noexcept : _error{code} {}
    constexpr Result(Error error) : _error(error) {}
    constexpr Result(Value &&value) : _error(OK), _value(std::move(value)) {}
    constexpr Result(Value const &value) : _error(OK), _value(value) {}

    constexpr explicit operator bool() { return bool(_error); }
    constexpr Error none() const { return _error; }
    constexpr Value &unwrap(char const *msg = "unwraping an error") { return _value.unwrap(msg); }
    constexpr Value take() { return _value.take(); }
};

static_assert(Tryable<Result<int>>);

} // namespace Karm
