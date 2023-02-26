
#include <karm-logger/logger.h>

#include "arch.h"
#include "sched.h"
#include "syscalls.h"

namespace Hjert::Core {

Hj::Code sysLog(char const *msg, size_t len) {
    logInfo("sysLog: '{}'", Str{msg, len});
    return Hj::OK;
}

Hj::Code sysCreate(Hj::Type type, Hj::Cap *cap, Hj::Arg a0, Hj::Arg a1, Hj::Arg a2, Hj::Arg a3) {
    (void)type;
    (void)cap;
    (void)a0;
    (void)a1;
    (void)a2;
    (void)a3;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysDrop(Hj::Cap cap) {
    (void)cap;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysDup(Hj::Cap node, Hj::Cap *dst, Hj::Cap src) {
    (void)node;
    (void)dst;
    (void)src;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysStart(Hj::Cap cap, uintptr_t ip, uintptr_t sp, Hj::Args const *args) {
    (void)cap;
    (void)ip;
    (void)sp;
    (void)args;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysWait(Hj::Cap cap, Hj::Arg *ret) {
    (void)cap;
    (void)ret;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysRet(Hj::Cap cap, Hj::Arg ret) {
    (void)cap;
    (void)ret;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysMap(Hj::Cap cap, uintptr_t virt, Hj::Cap mem, uintptr_t off, size_t len, Hj::MapFlags flags) {
    (void)cap;
    (void)virt;
    (void)mem;
    (void)off;
    (void)len;
    (void)flags;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysUnmap(Hj::Cap cap, uintptr_t virt, size_t len) {
    (void)cap;
    (void)virt;
    (void)len;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysIn(Hj::Cap cap, Hj::IoLen len, uintptr_t port, Hj::Arg *val) {
    (void)cap;
    (void)len;
    (void)port;
    (void)val;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysOut(Hj::Cap cap, Hj::IoLen len, uintptr_t port, Hj::Arg val) {
    (void)cap;
    (void)len;
    (void)port;
    (void)val;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code sysIpc(Hj::Cap *cap, Hj::Cap dst, Hj::Msg *msg, Hj::IpcFlags flags) {
    (void)cap;
    (void)dst;
    (void)msg;
    (void)flags;
    return Hj::Code::NOT_IMPLEMENTED;
}

Hj::Code dispatchSyscall(Hj::Syscall id, Hj::Args args) {
    switch (id) {
    case Hj::Syscall::LOG:
        return sysLog((char const *)args[0], args[1]);

    case Hj::Syscall::CREATE:
        return sysCreate((Hj::Type)args[0], (Hj::Cap *)args[1], args[2], args[3], args[4], args[5]);

    case Hj::Syscall::DROP:
        return sysDrop(Hj::Cap{args[0]});

    case Hj::Syscall::DUP:
        return sysDup(Hj::Cap{args[0]}, (Hj::Cap *)args[1], Hj::Cap{args[2]});

    case Hj::Syscall::START:
        return sysStart(Hj::Cap{args[0]}, args[1], args[2], (Hj::Args const *)args[3]);

    case Hj::Syscall::WAIT:
        return sysWait(Hj::Cap{args[0]}, (Hj::Arg *)args[1]);

    case Hj::Syscall::RET:
        return sysRet(Hj::Cap{args[0]}, args[1]);

    case Hj::Syscall::MAP:
        return sysMap(Hj::Cap{args[0]}, args[1], Hj::Cap{args[2]}, args[3], args[4], (Hj::MapFlags)args[5]);

    case Hj::Syscall::UNMAP:
        return sysUnmap(Hj::Cap{args[0]}, args[1], args[2]);

    case Hj::Syscall::IN:
        return sysIn(Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], (Hj::Arg *)args[3]);

    case Hj::Syscall::OUT:
        return sysOut(Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], args[3]);

    case Hj::Syscall::IPC:
        return sysIpc((Hj::Cap *)args[0], Hj::Cap{args[1]}, (Hj::Msg *)args[2], (Hj::IpcFlags)args[3]);

    default:
        return Hj::Code::BAD_SYSCALL;
    }
}

} // namespace Hjert::Core
