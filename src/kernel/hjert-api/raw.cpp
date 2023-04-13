#include "raw.h"

namespace Hj {

#ifdef __osdk_arch_x86_64__

Res<> _syscall(Syscall s, Arg a0, Arg a1, Arg a2, Arg a3, Arg a4, Arg a5) {
    // s : rax, arg1 : rdi, arg2 : rsi, arg3 : rdx, arg4 : r10, arg5 : r8, arg6 : r9
    Error::Code c = {};
    register Arg r3 asm("r10") = a3;
    register Arg r4 asm("r8") = a4;
    register Arg r5 asm("r9") = a5;
    asm volatile("syscall"
                 : "=a"(c)
                 : "a"(s), "D"(a0), "S"(a1), "d"(a2),
                   "r"(r3), "r"(r4), "r"(r5)
                 : "rcx", "r11", "memory");

    if (c != Error::Code::_OK)
        return Error(c);

    return Ok();
}

#else
#    error "Unsupported architecture"
#endif

Res<> _log(char const *msg, usize len) {
    return _syscall(Syscall::LOG, (Arg)msg, len);
}

Res<> _createDomain(Cap dest, Cap *cap) {
    return _syscall(Syscall::CREATE_DOMAIN, dest.raw(), (Arg)cap);
}

Res<> _createTask(Cap dest, Cap *cap, Cap node, Cap space) {
    return _syscall(Syscall::CREATE_TASK, dest.raw(), (Arg)cap, node.raw(), space.raw());
}

Res<> _createSpace(Cap dest, Cap *cap) {
    return _syscall(Syscall::CREATE_SPACE, dest.raw(), (Arg)cap);
}

Res<> _createMem(Cap dest, Cap *cap, usize phys, usize len, MemFlags flags) {
    return _syscall(Syscall::CREATE_MEM, dest.raw(), (Arg)cap, phys, len, (Arg)flags);
}

Res<> _createIo(Cap dest, Cap *cap, usize base, usize len) {
    return _syscall(Syscall::CREATE_IO, dest.raw(), (Arg)cap, base, len);
}

Res<> _label(Cap cap, char const *label, usize len) {
    return _syscall(Syscall::LABEL, cap.raw(), (Arg)label, len);
}

Res<> _drop(Cap cap) {
    return _syscall(Syscall::DROP, cap.raw());
}

Res<> _dup(Cap node, Cap *dst, Cap src) {
    return _syscall(Syscall::DUP, node.raw(), (Arg)dst, src.raw());
}

Res<> _start(Cap cap, usize ip, usize sp, Args const *args) {
    return _syscall(Syscall::START, cap.raw(), ip, sp, (Arg)args);
}

Res<> _wait(Cap cap, Arg *ret) {
    return _syscall(Syscall::WAIT, cap.raw(), (Arg)ret);
}

Res<> _ret(Cap cap, Arg ret) {
    return _syscall(Syscall::RET, cap.raw(), ret);
}

using MapFlags = Hal::VmmFlags;

Res<> _map(Cap cap, usize *virt, Cap mem, usize off, usize len, MapFlags flags) {
    return _syscall(Syscall::MAP, cap.raw(), (Arg)virt, mem.raw(), off, len, (Arg)flags);
}

Res<> _unmap(Cap cap, usize virt, usize len) {
    return _syscall(Syscall::UNMAP, cap.raw(), virt, len);
}

Res<> _in(Cap cap, IoLen len, usize port, Arg *val) {
    return _syscall(Syscall::IN, cap.raw(), (Arg)len, port, (Arg)val);
}

Res<> _out(Cap cap, IoLen len, usize port, Arg val) {
    return _syscall(Syscall::OUT, cap.raw(), (Arg)len, port, val);
}

Res<> _ipc(Cap *cap, Cap dst, Msg *msg, IpcFlags flags) {
    return _syscall(Syscall::IPC, (Arg)cap, dst.raw(), (Arg)msg, (Arg)flags);
}

} //  namespace Hj
