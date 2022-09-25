#pragma once

#include "view.h"

namespace Karm::Ui {

/* --- Input ----------------------------------------------------------------- */

struct Input : public View<Input> {
};

inline Child input() {
    return makeStrong<Input>();
}

/* --- Toggle --------------------------------------------------------------- */

struct Toggle : public View<Toggle> {
};

inline Child toggle() {
    return makeStrong<Toggle>();
}

} // namespace Karm::Ui
