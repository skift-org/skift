#pragma once

#include <karm-meta/callable.h>

#include "box.h"
#include "rc.h"

namespace Karm {

template <typename>
struct Func;

template <typename Out, typename... In>
struct Func<Out(In...)> {
    struct _Wrap {
        virtual ~_Wrap() = default;
        virtual Out operator()(In...) const = 0;
    };

    template <typename F>
    struct Wrap : _Wrap {
        F _f;

        Wrap(F &&f) : _f(std::move(f)) {}

        Out operator()(In... in) const override { return _f(std::forward<In>(in)...); }
    };

    Box<_Wrap> _wrap;

    Func() = delete;

    // clang-format off
    // clang-format has troubles with this

    template <typename F>
    requires Meta::FuncPtr<F> and Meta::Callable<F, In...>
    Func(F f) : _wrap(makeBox<Wrap<F>>(std::move(f))) {}

    template <typename F>
    requires Meta::RvalueRef<F &&> and (not Meta::FuncPtr<F>) and Meta::Callable<F, In...> and (not Meta::Same<Meta::RemoveConstVolatileRef<F>, Func>)
    Func(F &&f) : _wrap(makeBox<Wrap<F>>(std::move(f))) {}

    Func(Func &&other) : _wrap(std::move(other._wrap)) {}

    template <typename F>
    requires Meta::FuncPtr<F> and Meta::Callable<F, In...>
    Func &operator=(F f)
    {
        _wrap = makeBox<Wrap<F>>(std::move(f));
        return *this;
    }

    template <typename F>
    requires (Meta::RvalueRef<F &&> and (not Meta::FuncPtr<F>) and (not Meta::Same<Meta::RemoveConstVolatileRef<F>, Func>))
    Func &operator=(F &&f)
    {
        _wrap = makeBox<Wrap<F>>(std::move(f));
        return *this;
    }

    Func& operator=(Func &&other) {
        _wrap = std::move(other._wrap);
        return *this;
    }

    // clang-format on

    Out operator()(In... in) const {
        return (*_wrap)(std::forward<In>(in)...);
    }

    template <typename F>
    Func &operator=(F f) {
        _wrap = makeBox(Wrap<F>{std::move(f)});
        return *this;
    }
};

template <typename>
struct SharedFunc;

template <typename Out, typename... In>
struct SharedFunc<Out(In...)> {
    struct _Wrap {
        virtual ~_Wrap() = default;
        virtual Out operator()(In...) const = 0;
    };

    template <typename F>
    struct Wrap : _Wrap {
        F _f;

        Wrap(F &&f) : _f(std::move(f)) {}

        Out operator()(In... in) const override { return _f(std::forward<In>(in)...); }
    };

    Strong<_Wrap> _wrap;

    SharedFunc() = delete;

    // clang-format off
    // clang-format has troubles with this

    template <typename F>
    requires Meta::FuncPtr<F> and Meta::Callable<F, In...>
    SharedFunc(F f) : _wrap(makeStrong<Wrap<F>>(std::move(f))) {}

    template <typename F>
    requires (Meta::RvalueRef<F &&> and not (Meta::FuncPtr<F>)) and Meta::Callable<F, In...>
    SharedFunc(F &&f) : _wrap(makeStrong<Wrap<F>>(std::move(f))) {}

    template <typename F>
    requires Meta::FuncPtr<F>
    SharedFunc &operator=(F f)
    {
        _wrap = makeStrong<Wrap<F>>(std::move(f));
        return *this;
    }

    template <typename F>
    requires (Meta::RvalueRef<F &&> and not (Meta::FuncPtr<F>))
    SharedFunc &operator=(F &&f)
    {
        _wrap = makeStrong<Wrap<F>>(std::move(f));
        return *this;
    }

    // clang-format on

    Out operator()(In... in) const {
        return (*_wrap)(std::forward<In>(in)...);
    }

    template <typename F>
    SharedFunc &operator=(F f) {
        _wrap = makeStrong(Wrap<F>{std::move(f)});
        return *this;
    }
};

template <typename... Args>
auto bind(auto f, Args... args) {
    return [f = std::move(f), args...](auto &&...in) {
        return f(args..., std::forward<decltype(in)>(in)...);
    };
}

template <typename... Args>
auto rbind(auto f, Args... args) {
    return [f = std::move(f), args...](auto &&...in) {
        return f(std::forward<decltype(in)>(in)..., args...);
    };
}

} // namespace Karm
