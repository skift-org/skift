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

    constexpr Result(None) : _error(NONE), _value(NONE) {}

    constexpr Result(Error::Code code) : _error(code) {}

    constexpr Result(Error error) : _error(error) {}

    template <typename U = Value>
        requires(!Meta::Same<Meta::RemoveConstVolatileRef<U>, Result<Value>> and Meta::Constructible<Value, U &&>)
    constexpr Result(U &&value) : _error(OK), _value(std::forward<U>(value)) {}

    constexpr explicit operator bool() { return bool(_error); }

    constexpr Error none() const { return _error; }

    constexpr Value &unwrap(char const *msg = "unwraping an error") { return _value.unwrap(msg); }

    constexpr Value take() { return _value.take(); }
};

static_assert(Tryable<Result<int>>);

} // namespace Karm
