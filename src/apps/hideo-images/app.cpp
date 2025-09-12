export module Hideo.Images;

import Karm.Core;
import :editor;
import :viewer;

namespace Hideo::Images {

export Ui::Child app(State initial) {
    return Ui::reducer<Model>(
        initial,
        [](auto const& s) {
            return s.isEditor
                       ? editorApp(s)
                       : viewerApp(s);
        }
    );
}

} // namespace Hideo::Images
