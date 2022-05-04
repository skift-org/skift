#pragma once

#include "hook.h"

namespace Karm::Ui {

struct _EffectHook : public Hook {
};

void useEffect(Base::Func<Base::Func<void()>()>) {
}

} // namespace Karm::Ui
