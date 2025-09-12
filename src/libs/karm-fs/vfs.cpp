export module Karm.Fs:vfs;

import Karm.Core;
import :node;

namespace Karm::Fs {

export struct Vfs : Node {};

export struct VFile : Node {
    Vec<u8> _buf;
};

export struct VDir : Node {
    Map<String, Rc<Node>> _entries;
};

} // namespace Karm::Fs