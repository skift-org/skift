export module Vaev.Script:agent;

import Karm.Core;
import Karm.Gc;

using namespace Karm;

namespace Vaev::Script {

// https://tc39.es/ecma262/#agent
export struct Agent {
    Gc::Heap& heap;
};

} // namespace Vaev::Script
