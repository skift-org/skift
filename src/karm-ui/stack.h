#pragma once

#include "widget.h"

namespace Karm::Ui {

struct _VStack : public _Widget {
};

void vstack(Children children = {}) {
    widget([&] {
        children();
    });
}

struct _HStack : public _Widget {
};

void hstack(Children children = {}) {
    widget([&] {
        children();
    });
}

struct _ZStack : public _Widget {
};

void zstack(Children children = {}) {
    widget([&] {
        children();
    });
}

} // namespace Karm::Ui
