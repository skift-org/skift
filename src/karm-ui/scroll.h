#pragma once

#include "widget.h"

namespace Karm::Ui {

struct _VScroll : public _Widget {
};

void vscroll(Children children = {}) {
    widget([&] {
        children();
    });
}

struct _HScroll : public _Widget {
};

void hscroll(Children children = {}) {
    widget([&] {
        children();
    });
}

struct _VHScroll : public _Widget {
};

void vhscroll(Children children = {}) {
    widget([&] {
        children();
    });
}

} // namespace Karm::Ui
