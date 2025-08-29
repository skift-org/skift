module;

#include <hjert-core/arch.h>
#include <hjert-core/cpu.h>
#include <karm-logger/logger.h>

module Karm.Core;

namespace Karm::_Embed {

void relaxe() { Hjert::Arch::globalCpu().relaxe(); }

void enterCritical() { Hjert::Arch::globalCpu().retainInterrupts(); }

void leaveCritical() { Hjert::Arch::globalCpu().releaseInterrupts(); }

} // namespace Karm::_Embed
