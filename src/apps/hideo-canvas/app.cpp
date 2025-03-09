module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/draw.h>

export module Hideo.Canvas;

namespace Hideo::Canvas {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::DRAW,
        .title = "Canvas"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Canvas
