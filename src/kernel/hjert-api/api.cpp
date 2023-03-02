#include "api.h"

namespace Hj {

#ifdef __osdk_arch_x86_64__

Res<> syscall(Syscall s, Arg a0, Arg a1, Arg a2, Arg a3, Arg a4, Arg a5) {
    Error::Code c;

    // s : rax, arg1 : rdi, arg2 : rsi, arg3 : rdx, arg4 : r10, arg5 : r8, arg6 : r9
    __asm__ volatile(
        "syscall"
        : "=a"(c)
        : "a"(s), "D"(a0), "S"(a1), "d"(a2), "r"(a3), "r"(a4), "r"(a5)
        : "rcx", "r11", "memory");

    if (c != Error::Code::_OK)
        return Error(c);

    return Ok();
}

#else
#    error "Unsupported architecture"
#endif

Res<> log(char const *msg, usize len) {
    return syscall(Syscall::LOG, (Arg)msg, len);
}

Res<> createDomain(Cap dest, Cap *cap, usize len) {
    return syscall(Syscall::CREATE_DOMAIN, dest.raw(), (Arg)cap, len);
}

Res<> createTask(Cap dest, Cap *cap, Cap node, Cap space) {
    return syscall(Syscall::CREATE_TASK, dest.raw(), (Arg)cap, node.raw(), space.raw());
}

Res<> createSpace(Cap dest, Cap *cap) {
    return syscall(Syscall::CREATE_SPACE, dest.raw(), (Arg)cap);
}

Res<> createMem(Cap dest, Cap *cap, usize phys, usize len, MemFlags flags) {
    return syscall(Syscall::CREATE_MEM, dest.raw(), (Arg)cap, phys, len, (Arg)flags);
}

Res<> createIo(Cap dest, Cap *cap, usize base, usize len) {
    return syscall(Syscall::CREATE_IO, dest.raw(), (Arg)cap, base, len);
}

Res<> drop(Cap cap) {
    return syscall(Syscall::DROP, cap.raw());
}

Res<> dup(Cap node, Cap *dst, Cap src) {
    return syscall(Syscall::DUP, node.raw(), (Arg)dst, src.raw());
}

Res<> start(Cap cap, usize ip, usize sp, Args const *args) {
    return syscall(Syscall::START, cap.raw(), ip, sp, (Arg)args);
}

Res<> wait(Cap cap, Arg *ret) {
    return syscall(Syscall::WAIT, cap.raw(), (Arg)ret);
}

Res<> ret(Cap cap, Arg ret) {
    return syscall(Syscall::RET, cap.raw(), ret);
}

using MapFlags = Hal::VmmFlags;

Res<> map(Cap cap, usize *virt, Cap mem, usize off, usize len, MapFlags flags) {
    return syscall(Syscall::MAP, cap.raw(), (Arg)virt, mem.raw(), off, len, (Arg)flags);
}

Res<> unmap(Cap cap, usize virt, usize len) {
    return syscall(Syscall::UNMAP, cap.raw(), virt, len);
}

Res<> in(Cap cap, IoLen len, usize port, Arg *val) {
    return syscall(Syscall::IN, cap.raw(), (Arg)len, port, (Arg)val);
}

Res<> out(Cap cap, IoLen len, usize port, Arg val) {
    return syscall(Syscall::OUT, cap.raw(), (Arg)len, port, val);
}

Res<> ipc(Cap *cap, Cap dst, Msg *msg, IpcFlags flags) {
    return syscall(Syscall::IPC, (Arg)cap, dst.raw(), (Arg)msg, (Arg)flags);
}

} //  namespace Hj
