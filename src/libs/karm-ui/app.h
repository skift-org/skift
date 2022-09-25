#pragma once

#include <embed/ui.h>
#include <karm-main/base.h>

#include "host.h"
#include "state.h"
#include "supplier.h"

namespace Karm::Ui {

struct App {
    Child _root;

    App(Child root)
        : _root(root) {
    }

    App(Func<Child()> root)
        : _root(root()) {
    }

    template <typename T>
    App(T initial, Func<Child(State<T>)> root)
        : _root(state(initial, std::move(root))) {
    }

    template <typename T, typename... Args>
    App &supply(Args... args) {
        _root = makeStrong<Supplier<T>>(_root, std::forward<Args>(args)...);
        return *this;
    }

    CliResult run(CliArgs args) {
        supply<CliArgs>(args);
        return try$(Embed::makeHost(_root))->run();
    }
};

template <typename T, typename... Args>
inline CliResult runApp(CliArgs cliArgs, Args... args) {
    return App{makeStrong<T>(std::forward<Args>(args)...)}.run(cliArgs);
}

inline CliResult runApp(CliArgs args, Child root) {
    return App{root}.run(args);
}

} // namespace Karm::Ui
