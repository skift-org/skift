
#include <karm-logger/logger.h>

#include "arch.h"
#include "io.h"
#include "sched.h"
#include "syscalls.h"

namespace Hjert::Core {

Res<> doLog(char const *msg, size_t len) {
    logInfo("doLog: '{}'", Str{msg, len});
    return Ok();
}

Res<> doCreateDomain(Hj::Cap dest, Hj::Cap *cap, size_t len) {
    (void)dest;
    (void)cap;
    (void)len;
    return Error::notImplemented();
}

Res<> doCreateTask(Hj::Cap dest, Hj::Cap *cap, Hj::Cap domain, Hj::Cap space) {
    (void)dest;
    (void)cap;
    (void)domain;
    (void)space;
    return Error::notImplemented();
}

Res<> doCreateSpace(Hj::Cap dest, Hj::Cap *cap) {
    (void)dest;
    (void)cap;
    return Error::notImplemented();
}

Res<> doCreateMem(Hj::Cap dest, Hj::Cap *cap, uintptr_t phys, size_t len, Hj::MemFlags flags) {
    (void)dest;
    (void)cap;
    (void)phys;
    (void)len;
    (void)flags;
    return Error::notImplemented();
}

Res<> doCreateIo(Hj::Cap dest, Hj::Cap *cap, uintptr_t base, size_t len) {
    (void)dest;
    (void)cap;
    (void)base;
    (void)len;
    return Error::notImplemented();
}

Res<> doDrop(Hj::Cap cap) {
    (void)cap;
    return Error::notImplemented();
}

Res<> doDup(Hj::Cap node, Hj::Cap *dst, Hj::Cap src) {
    (void)node;
    (void)dst;
    (void)src;
    return Error::notImplemented();
}

Res<> doStart(Hj::Cap cap, uintptr_t ip, uintptr_t sp, Hj::Args const *args) {
    (void)cap;
    (void)ip;
    (void)sp;
    (void)args;
    return Error::notImplemented();
}

Res<> doWait(Hj::Cap cap, Hj::Arg *ret) {
    (void)cap;
    (void)ret;
    return Error::notImplemented();
}

Res<> doRet(Hj::Cap cap, Hj::Arg ret) {
    (void)cap;
    (void)ret;
    return Error::notImplemented();
}

Res<> doMap(Hj::Cap cap, uintptr_t virt, Hj::Cap mem, uintptr_t off, size_t len, Hj::MapFlags flags) {
    (void)cap;
    (void)virt;
    (void)mem;
    (void)off;
    (void)len;
    (void)flags;
    return Error::notImplemented();
}

Res<> doUnmap(Hj::Cap cap, uintptr_t virt, size_t len) {
    (void)cap;
    (void)virt;
    (void)len;
    return Error::notImplemented();
}

Res<> doIn(Hj::Cap cap, Hj::IoLen len, uintptr_t port, Hj::Arg *val) {
    (void)cap;
    (void)len;
    (void)port;
    (void)val;
    return Error::notImplemented();
}

Res<> doOut(Hj::Cap cap, Hj::IoLen len, uintptr_t port, Hj::Arg val) {
    (void)cap;
    (void)len;
    (void)port;
    (void)val;
    return Error::notImplemented();
}

Res<> doIpc(Hj::Cap *cap, Hj::Cap dst, Hj::Msg *msg, Hj::IpcFlags flags) {
    (void)cap;
    (void)dst;
    (void)msg;
    (void)flags;
    return Error::notImplemented();
}

Res<> dispatchSyscall(Hj::Syscall id, Hj::Args args) {
    switch (id) {
    case Hj::Syscall::LOG:
        return doLog((char const *)args[0], args[1]);

    case Hj::Syscall::CREATE_DOMAIN:
        return doCreateDomain(Hj::Cap{args[0]}, (Hj::Cap *)args[1], args[2]);

    case Hj::Syscall::CREATE_TASK:
        return doCreateTask(Hj::Cap{args[0]}, (Hj::Cap *)args[1], Hj::Cap{args[2]}, Hj::Cap{args[3]});

    case Hj::Syscall::CREATE_SPACE:
        return doCreateSpace(Hj::Cap{args[0]}, (Hj::Cap *)args[1]);

    case Hj::Syscall::CREATE_MEM:
        return doCreateMem(Hj::Cap{args[0]}, (Hj::Cap *)args[1], args[2], args[3], Hj::MemFlags{args[4]});

    case Hj::Syscall::CREATE_IO:
        return doCreateIo(Hj::Cap{args[0]}, (Hj::Cap *)args[1], args[2], args[3]);

    case Hj::Syscall::DROP:
        return doDrop(Hj::Cap{args[0]});

    case Hj::Syscall::DUP:
        return doDup(Hj::Cap{args[0]}, (Hj::Cap *)args[1], Hj::Cap{args[2]});

    case Hj::Syscall::START:
        return doStart(Hj::Cap{args[0]}, args[1], args[2], (Hj::Args const *)args[3]);

    case Hj::Syscall::WAIT:
        return doWait(Hj::Cap{args[0]}, (Hj::Arg *)args[1]);

    case Hj::Syscall::RET:
        return doRet(Hj::Cap{args[0]}, args[1]);

    case Hj::Syscall::MAP:
        return doMap(Hj::Cap{args[0]}, args[1], Hj::Cap{args[2]}, args[3], args[4], (Hj::MapFlags)args[5]);

    case Hj::Syscall::UNMAP:
        return doUnmap(Hj::Cap{args[0]}, args[1], args[2]);

    case Hj::Syscall::IN:
        return doIn(Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], (Hj::Arg *)args[3]);

    case Hj::Syscall::OUT:
        return doOut(Hj::Cap{args[0]}, (Hj::IoLen)args[1], args[2], args[3]);

    case Hj::Syscall::IPC:
        return doIpc((Hj::Cap *)args[0], Hj::Cap{args[1]}, (Hj::Msg *)args[2], (Hj::IpcFlags)args[3]);

    default:
        return Error::invalidInput("invalid syscall id");
    }
}

} // namespace Hjert::Core
