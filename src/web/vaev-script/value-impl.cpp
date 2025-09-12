module Vaev.Script;

import Karm.Core;

namespace Vaev::Script {

void Value::repr(Io::Emit& e) const {
    e("{}", store);
}

} // namespace Vaev::Script