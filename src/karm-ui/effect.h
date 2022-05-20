#pragma once

#include <karm-base/func.h>

#include "hook.h"
#include "ui.h"

namespace Karm::Ui {

struct Effect : public Hook {
    Func<Func<void()>()> _effect;
    Opt<Func<void()>> _cleanup;

    virtual void onMount() {
        _cleanup = _effect();
    }

    virtual void onUnmount() {
        if (_cleanup) {
            _cleanup.unwrap()();
        }
    }
};

static Effect &useEffect(Func<void()>) {
    return ctx().hook<Effect>();
}

static Effect &useEffect(Func<Func<void()>()> effect) {
    return ctx().hook<Effect>(std::move(effect));
}

} // namespace Karm::Ui
