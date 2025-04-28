#include <seccomp.h>

//
#include <karm-sys/_embed.h>

#include "../utils.h"

namespace Karm::Sys::_Embed {

#define FOREACH_SYSCALLS(SYSCALL) \
    SYSCALL(access)               \
    SYSCALL(brk)                  \
    SYSCALL(close)                \
    SYSCALL(exit_group)           \
    SYSCALL(fstat)                \
    SYSCALL(futex)                \
    SYSCALL(getcwd)               \
    SYSCALL(getdents64)           \
    SYSCALL(getrandom)            \
    SYSCALL(io_uring_enter)       \
    SYSCALL(io_uring_setup)       \
    SYSCALL(epoll_ctl)            \
    SYSCALL(epoll_pwait)          \
    SYSCALL(lseek)                \
    SYSCALL(mmap)                 \
    SYSCALL(mprotect)             \
    SYSCALL(munmap)               \
    SYSCALL(newfstatat)           \
    SYSCALL(fstat)                \
    SYSCALL(openat)               \
    SYSCALL(read)                 \
    SYSCALL(write)

Res<> hardenSandbox() {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL_PROCESS);

    if (!ctx)
        return Posix::fromLastErrno();

    Defer cleanupSeccomp = [&] {
        seccomp_release(ctx);
    };

#define ITER(SYSCALL)                                                                    \
    if (auto it = seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(SYSCALL), 0); it < 0) { \
        return Posix::fromErrno(-it);                                                    \
    }
    FOREACH_SYSCALLS(ITER)
#undef ITER

    if (seccomp_load(ctx) < 0)
        return Posix::fromLastErrno();

    return Ok();
}

} // namespace Karm::Sys::_Embed
