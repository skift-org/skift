#pragma once

#include <karm-base/string.h>

namespace Karm::Ui {

struct Hook {
    virtual ~Hook() = default;

    virtual Str desc() const = 0;

    virtual void onMount() {}

    virtual void onUnmount() {}
};

} // namespace Karm::Ui
