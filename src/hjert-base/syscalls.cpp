#include <hjert-api/syscalls.h>
#include <hjert-arch/arch.h>

namespace Hjert {

Karm::Error handle(Api::Log &log) {
    return Arch::writeLog(log.buf, log.len).none();
}

Karm::Error handle(Api::Id id, Api::Arg args) {
}

} // namespace Hjert
