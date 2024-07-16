#include "syscalls.h"

namespace Hj {

#ifdef __ck_arch_x86_64__

Res<> _syscall(Syscall s, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0, Arg a4 = 0, Arg a5 = 0) {
    Error::Code c = {};
    register Arg r3 asm("r10") = a3;
    register Arg r4 asm("r8") = a4;
    register Arg r5 asm("r9") = a5;
    asm volatile("syscall"
                 : "=a"(c)
                 : "a"(s), "D"(a0), "S"(a1), "d"(a2), "r"(r3), "r"(r4), "r"(r5)
                 : "memory", "r11", "rcx");

    if (c != Error::Code::_OK)
        return Error(c, nullptr);

    return Ok();
}

#elif defined(__ck_arch_arm64__)

Res<> _syscall(Syscall s, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0, Arg a4 = 0, Arg a5 = 0) {
    Error::Code c = {};
    register Arg x8 asm("x8") = (Arg)s;
    register Arg x0 asm("x0") = a0;
    register Arg x1 asm("x1") = a1;
    register Arg x2 asm("x2") = a2;
    register Arg x3 asm("x3") = a3;
    register Arg x4 asm("x4") = a4;
    register Arg x5 asm("x5") = a5;
    asm volatile("svc #0"
                 : "=r"(c)
                 : "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
                 : "memory");

    if (c != Error::Code::_OK)
        return Error(c, nullptr);

    return Ok();
}

#elif defined(__ck_arch_riscv32__) || defined(__ck_arch_riscv64__)

Res<> _syscall(Syscall s, Arg a0 = 0, Arg a1 = 0, Arg a2 = 0, Arg a3 = 0, Arg a4 = 0, Arg a5 = 0) {
    Error::Code c = {};
    register Arg a7 asm("a7") = (Arg)s;
    register Arg a0 asm("a0") = a0;
    register Arg a1 asm("a1") = a1;
    register Arg a2 asm("a2") = a2;
    register Arg a3 asm("a3") = a3;
    register Arg a4 asm("a4") = a4;
    register Arg a5 asm("a5") = a5;
    asm volatile("ecall"
                 : "=r"(c)
                 : "r"(a7), "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5)
                 : "memory");

    if (c != Error::Code::_OK)
        return Error(c, nullptr);

    return Ok();
}

#else
#    error "Unsupported architecture"
#endif

Res<> _now(TimeStamp *ts) {
    return _syscall(Syscall::NOW, (Arg)ts);
}

Res<> _log(char const *msg, usize len) {
    return _syscall(Syscall::LOG, (Arg)msg, len);
}

Res<> _create(Cap dest, Cap *cap, Props const *props) {
    return _syscall(Syscall::CREATE, dest.raw(), (Arg)cap, (Arg)props);
}

Res<> _label(Cap cap, char const *label, usize len) {
    return _syscall(Syscall::LABEL, cap.raw(), (Arg)label, len);
}

Res<> _drop(Cap cap) {
    return _syscall(Syscall::DROP, cap.raw());
}

Res<> _pledge(Cap cap, Flags<Pledge> pledges) {
    return _syscall(Syscall::PLEDGE, cap.raw(), (Arg)pledges.val());
}

Res<> _dup(Cap node, Cap *dst, Cap src) {
    return _syscall(Syscall::DUP, node.raw(), (Arg)dst, src.raw());
}

Res<> _start(Cap cap, usize ip, usize sp, Args const *args) {
    return _syscall(Syscall::START, cap.raw(), ip, sp, (Arg)args);
}

Res<> _map(Cap cap, usize *virt, Cap vmo, usize off, usize *len, MapFlags flags) {
    return _syscall(Syscall::MAP, cap.raw(), (Arg)virt, vmo.raw(), off, (Arg)len, (Arg)flags);
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

Res<> _send(Cap cap, Byte const *buf, usize bufLen, Cap const *caps, usize capLen) {
    return _syscall(Syscall::SEND, cap.raw(), (Arg)buf, bufLen, (usize)caps, capLen);
}

Res<> _recv(Cap cap, Byte *buf, usize *bufLen, Cap *caps, usize *capLen) {
    return _syscall(Syscall::RECV, cap.raw(), (Arg)buf, (Arg)bufLen, (Arg)caps, (Arg)capLen);
}

Res<> _close(Cap cap) {
    return _syscall(Syscall::CLOSE, cap.raw());
}

Res<> _signal(Cap cap, Flags<Sigs> set, Flags<Sigs> unset) {
    return _syscall(Syscall::SIGNAL, cap.raw(), (Arg)set.val(), (Arg)unset.val());
}

Res<> _listen(Cap cap, Cap target, Flags<Sigs> set, Flags<Sigs> unset) {
    return _syscall(Syscall::LISTEN, cap.raw(), target.raw(), (Arg)set.val(), (Arg)unset.val());
}

Res<> _poll(Cap cap, Event *ev, usize evCap, usize *evLen, TimeStamp until) {
    return _syscall(Syscall::POLL, cap.raw(), (Arg)ev, evCap, (usize)evLen, until.val());
}

} //  namespace Hj
