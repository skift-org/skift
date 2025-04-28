module;

#include <karm-base/string.h>

export module Hideo.Images;

import :editor;
import :viewer;

namespace Hideo::Images {

export Ui::Child app(State initial) {
    return Ui::reducer<Model>(
        initial,
        [](auto const& s) {
            return Kr::scaffold({
                .icon = Mdi::IMAGE,
                .title = "Images"s,
                .body = [&] {
                    return s.isEditor
                               ? editor(s)
                               : viewer(s);
                },
            });
        }
    );
}

} // namespace Hideo::Images