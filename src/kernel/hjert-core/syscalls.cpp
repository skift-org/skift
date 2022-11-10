#include <hjert-api/syscalls.h>

#include "arch.h"

namespace Hjert {

static Error handle(Api::Create &) {

    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Grant &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Ref &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Deref &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Start &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Exit &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Pledge &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Map &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Unmap &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Ipc &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Bind &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Unbind &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Ack &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Io &) {
    return Error::NOT_IMPLEMENTED;
}

static Error handle(Api::Log &args) {
    try$(Arch::loggerOut()
             .write({(Byte *)args.buf, args.len}));
    return OK;
}

static Error handle(Api::Now &) {
    return Error::NOT_IMPLEMENTED;
}

Error handleSyscall(Api::SyscallId id, Api::Arg args) {
    Error result = Error::INVALID_INPUT;

    Api::Syscalls::visit((void *)args, [&](auto &syscall) {
        if (id == syscall.ID) {
            result = handle(syscall);
        }
    });

    return result;
}

} // namespace Hjert
