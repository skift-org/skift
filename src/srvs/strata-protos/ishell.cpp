module;

#include <karm-math/rect.h>
#include <karm-math/vec.h>

export module Strata.Protos:ishell;

import Karm.Sys;
import Karm.App;
import :base;

using namespace Karm;

namespace Strata::IShell {

struct WindowProps {
    Math::Vec2i size;
};

export using WindowId = usize;

export struct WindowCreate {
    using Response = Tuple<WindowId, WindowProps>;
    WindowProps want;
};

export struct WindowDestroy {
    using Response = None;
    WindowId window;
};

export struct WindowUpdate {
    WindowId window;
    WindowProps props;
};

export struct WindowAttach {
    WindowId window;
    Opt<Rc<Protos::Surface>> buffer;
};

export struct WindowEvent {
    WindowId window;
    Union<None, App::MouseEvent> event;
};

export struct WindowFlip {
    using Response = None;

    WindowId window;
    Math::Recti region;
};

} // namespace Strata::IShell
