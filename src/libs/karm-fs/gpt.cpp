export module Karm.Fs:gpt;

import Karm.Core;
import :node;

namespace Karm::Fs {

export struct Gpt : Node {
    Rc<Node> _volume;

    static Async::Task<Rc<Gpt>> formatAsync(Rc<Node> volume);
};

} // namespace Karm::Fs