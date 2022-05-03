#pragma once

#include "label.h"
#include "widget.h"

namespace Karm::Ui {

struct _Button : public _Widget {
};

void button(EventFunc, Children children = {}) {
    widget([&] {
        children();
    });
}

void button(Text::Str text, EventFunc event) {
    button(std::forward<EventFunc>(event), [&] {
        label(text);
    });
}

} // namespace Karm::Ui
