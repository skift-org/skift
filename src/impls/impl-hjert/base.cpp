#include <hjert-core/arch.h>
#include <hjert-core/cpu.h>
#include <karm-io/fmt.h>
#include <karm-logger/logger.h>

namespace Karm::_Embed {

void relaxe() { Hjert::Arch::cpu().relaxe(); }

void enterCritical() { Hjert::Arch::cpu().retainInterrupts(); }

void leaveCritical() { Hjert::Arch::cpu().releaseInterrupts(); }

} // namespace Karm::_Embed
