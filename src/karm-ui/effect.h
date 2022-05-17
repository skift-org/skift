#pragma once

#include <karm-base/func.h>

#include "hook.h"

namespace Karm::Ui {

struct _EffectHook : public Hook {
};

static void useEffect(Func<void()>) {
}

static void useEffect(Func<Func<void()>()>) {
}

} // namespace Karm::Ui
