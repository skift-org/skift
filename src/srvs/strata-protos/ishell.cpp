export module Strata.Protos:ishell;

import Karm.Math;
import Karm.Sys;
import Karm.App.Base;
import :base;

using namespace Karm;

namespace Strata::IShell {

export struct WindowProps {
    Math::Vec2i size;
    App::FormFactor formFactor;
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
    using Response = None;

    WindowId window;
    Opt<Rc<Protos::Surface>> buffer;
};

export struct WindowEvent {
    WindowId window;
    Union<None, App::MouseEvent, App::KeyboardEvent> event;
};

export struct WindowFlip {
    using Response = None;

    WindowId window;
    Math::Recti region;
};

export struct WindowMove {
    WindowId window;
};

} // namespace Strata::IShell
