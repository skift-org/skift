module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/presentation.h>

export module Hideo.Slides;

namespace Hideo::Slides {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::PRESENTATION,
        .title = "Slides"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Slides
