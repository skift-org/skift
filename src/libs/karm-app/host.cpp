export module Karm.App:host;

import Karm.Core;
import :event;

namespace Karm::App {

export struct RequestExitEvent {
    Res<> res = Ok();
};

export struct RequestMinimizeEvent {
};

export struct RequestMaximizeEvent {
};

} // namespace Karm::App
