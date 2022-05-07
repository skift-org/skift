#pragma once

#include <karm-text/str.h>

namespace Karm::Ui {

struct Hook {
    virtual ~Hook() = default;

    virtual Text::Str desc() const = 0;

    virtual void onMount(){};

    virtual void onUnmount(){};
};

} // namespace Karm::Ui
