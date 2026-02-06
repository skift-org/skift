module Karm.Core;

import Hjert.Api;

namespace Karm::_Embed {

void relaxe() {
#if defined(__x86_64__)
    asm volatile("pause");
#else
#    error "Unsupported architecture"
#endif
}

void enterCritical() {
}

void leaveCritical() {
}

} // namespace Karm::_Embed
