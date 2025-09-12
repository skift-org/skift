#pragma once

import Karm.Core;

#include "async.h"

namespace Karm::Sys {

struct Service {
    virtual ~Service() = default;
};

struct Context :
    Meta::Pinned {
    Vec<Rc<Service>> _srvs;
    static Context* _root;

    Context() {
        if (not _root)
            _root = this;
    }

    template <typename T>
    T& use() {
        for (auto& hook : _srvs)
            if (hook.is<T>())
                return hook.unwrap<T>();

        panic("no such service");
    }

    template <typename T, typename... Args>
    void add(Args&&... args) {
        _srvs.pushBack(makeRc<T>(std::forward<Args>(args)...));
    }
};

static inline Context& rootContext() {
    if (not Context::_root)
        panic("no global context");
    return *Context::_root;
}

struct ArgsHook :
    Service {

    usize _argc;
    char const** _argv;

    ArgsHook(usize argc, char const** argv)
        : _argc(argc), _argv(argv) {}

    Str self() const {
        return _argv[0];
    }

    usize len() const {
        return _argc - 1;
    }

    Str operator[](usize i) const {
        if (i >= len())
            panic("index out of bounds");
        return _argv[i + 1];
    }

    bool has(Str arg) const {
        for (usize i = 0; i < len(); ++i)
            if (operator[](i) == arg)
                return true;
        return false;
    }
};

inline auto& useArgs(Context& ctx) {
    return ctx.use<ArgsHook>();
}

using EntryPointAsync = Async::Task<>(Sys::Context&);

} // namespace Karm::Sys

[[gnu::used]] Karm::Async::Task<> entryPointAsync(Karm::Sys::Context&);
