#pragma once

#include <karm-meta/callable.h>

#include "_prelude.h"

#include "box.h"

namespace Karm {

template <typename>
struct Func;

template <typename Out, typename... In>
struct Func<Out(In...)> {
    struct _Wrap {
        virtual ~_Wrap() = default;
        virtual Out operator()(In...) = 0;
    };

    template <typename F>
    struct Wrap : _Wrap {
        F _f;
        Wrap(F &&f) : _f(std::move(f)) {}
        Out operator()(In... in) override { return _f(std::move(in)...); }
    };

    Box<_Wrap> _wrap;

    Func() = delete;

    // clang-format off
    // clang-format has troubles with this

    template <typename F>
    requires Meta::FuncPtr<F>
    Func(F f) : _wrap(makeBox<Wrap<F>>(std::move(f))) {}

    template <typename F>
    requires (Meta::RvalueRef<F &&> && !(Meta::FuncPtr<F>))
    Func(F &&f) : _wrap(makeBox<Wrap<F>>(std::move(f))) {}

    template <typename F>
    requires Meta::FuncPtr<F>
    Func &operator=(F f)
    {
        _wrap = makeBox<Wrap<F>>(std::move(f));
        return *this;
    }

    template <typename F>
    requires (Meta::RvalueRef<F &&> && !(Meta::FuncPtr<F>))
    Func &operator=(F &&f)
    {
        _wrap = makeBox<Wrap<F>>(std::move(f));
        return *this;
    }

    // clang-format on

    Out operator()(In... in) {
        return (*_wrap)(std::forward<In>(in)...);
    }

    template <typename F>
    Func &operator=(F f) {
        _wrap = makeBox(Wrap<F>{std::move(f)});
        return *this;
    }
};

} // namespace Karm
