#include <hjert-api/syscalls.h>

#include "arch.h"

namespace Hjert {

static Error handle(Api::Log &log) {
    return Arch::loggerOut().write({(Byte *)log.buf, log.len}).none();
}

Error handleSyscall(Api::Id id, Api::Arg args) {
    Error result = Error::INVALID_INPUT;

    Api::Syscalls::visit((void *)args, [&](auto &syscall) {
        if (id == syscall.ID) {
            result = handle(syscall);
        }
    });

    return result;
}

} // namespace Hjert
