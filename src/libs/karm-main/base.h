#pragma once

#include <karm-base/rc.h>
#include <karm-base/res.h>
#include <karm-base/vec.h>

struct Hook {
    virtual ~Hook() = default;
};

struct Ctx :
    public Meta::Static {
    Vec<Strong<Hook>> _hooks;
    static Ctx *_ctx;
    static Ctx &instance() {
        return *_ctx;
    }

    Ctx() {
        _ctx = this;
    }

    template <typename T>
    T &use() {
        for (auto &hook : _hooks) {
            if (hook.is<T>()) {
                return hook.unwrap<T>();
            }
        }

        panic("no such hook");
    }

    template <typename T, typename... Args>
    void add(Args &&...args) {
        _hooks.pushBack(makeStrong<T>(std::forward<Args>(args)...));
    }
};

struct ArgsHook :
    public Hook {

    isize _argc;
    char const **_argv;

    ArgsHook(isize argc, char const **argv)
        : _argc(argc), _argv(argv) {}

    Str self() const {
        return _argv[0];
    }

    usize len() const {
        return _argc - 1;
    }

    Str operator[](usize i) const {
        if (i >= len()) {
            panic("out of range");
        }
        return _argv[i + 1];
    }

    bool has(Str arg) const {
        for (usize i = 0; i < len(); ++i) {
            if (Op::eq(operator[](i), arg)) {
                return true;
            }
        }
        return false;
    }
};

inline auto &useArgs(Ctx &ctx = Ctx::instance()) {
    return ctx.use<ArgsHook>();
}

Res<> entryPoint(Ctx &ctx);
