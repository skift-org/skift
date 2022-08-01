#pragma once

#include "proxy.h"
#include "text.h"

namespace Karm::Ui {

struct Button : public Proxy<Button> {
    Func<void()> _onPress;

    Button(Func<void()> onPress, Child child)
        : Proxy(child), _onPress(std::move(onPress)) {}
};

Child button(Func<void()> onPress, Child child) {
    return makeStrong<Button>(std::move(onPress), child);
}

Child button(Func<void()> onPress, String t) {
    return button(std::move(onPress), text(t));
}

} // namespace Karm::Ui
