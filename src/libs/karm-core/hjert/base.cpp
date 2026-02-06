module Karm.Core;

import Hjert.Core;
import Karm.Logger;

namespace Karm::_Embed {

void relaxe() { Hjert::Arch::globalCpu().relaxe(); }

void enterCritical() { Hjert::Arch::globalCpu().retainInterrupts(); }

void leaveCritical() { Hjert::Arch::globalCpu().releaseInterrupts(); }

} // namespace Karm::_Embed
