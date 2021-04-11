#pragma once

#include <libc/__libc__.h>

#include <libsystem/Common.h>
#include <libsystem/Result.h>

#include <abi/Handle.h>
#include <abi/IOCall.h>
#include <abi/Launchpad.h>
#include <abi/System.h>

#define SYSCALL_LIST(__ENTRY)     \
    __ENTRY(HJ_PROCESS_THIS)      \
    __ENTRY(HJ_PROCESS_NAME)      \
    __ENTRY(HJ_PROCESS_LAUNCH)    \
    __ENTRY(HJ_PROCESS_CLONE)     \
    __ENTRY(HJ_PROCESS_EXEC)      \
    __ENTRY(HJ_PROCESS_EXIT)      \
    __ENTRY(HJ_PROCESS_CANCEL)    \
    __ENTRY(HJ_PROCESS_SLEEP)     \
    __ENTRY(HJ_PROCESS_WAIT)      \
    __ENTRY(HJ_MEMORY_ALLOC)      \
    __ENTRY(HJ_MEMORY_MAP)        \
    __ENTRY(HJ_MEMORY_FREE)       \
    __ENTRY(HJ_MEMORY_INCLUDE)    \
    __ENTRY(HJ_MEMORY_GET_HANDLE) \
    __ENTRY(HJ_FILESYSTEM_LINK)   \
    __ENTRY(HJ_FILESYSTEM_UNLINK) \
    __ENTRY(HJ_FILESYSTEM_RENAME) \
    __ENTRY(HJ_FILESYSTEM_MKPIPE) \
    __ENTRY(HJ_FILESYSTEM_MKDIR)  \
    __ENTRY(HJ_SYSTEM_INFO)       \
    __ENTRY(HJ_SYSTEM_STATUS)     \
    __ENTRY(HJ_SYSTEM_TIME)       \
    __ENTRY(HJ_SYSTEM_TICKS)      \
    __ENTRY(HJ_SYSTEM_REBOOT)     \
    __ENTRY(HJ_SYSTEM_SHUTDOWN)   \
    __ENTRY(HJ_HANDLE_OPEN)       \
    __ENTRY(HJ_HANDLE_CLOSE)      \
    __ENTRY(HJ_HANDLE_REOPEN)     \
    __ENTRY(HJ_HANDLE_COPY)       \
    __ENTRY(HJ_HANDLE_POLL)       \
    __ENTRY(HJ_HANDLE_READ)       \
    __ENTRY(HJ_HANDLE_WRITE)      \
    __ENTRY(HJ_HANDLE_CALL)       \
    __ENTRY(HJ_HANDLE_SEEK)       \
    __ENTRY(HJ_HANDLE_STAT)       \
    __ENTRY(HJ_HANDLE_CONNECT)    \
    __ENTRY(HJ_HANDLE_ACCEPT)     \
    __ENTRY(HJ_CREATE_PIPE)       \
    __ENTRY(HJ_CREATE_TERM)

#define SYSCALL_ENUM_ENTRY(__entry) __entry,

enum Syscall
{
    SYSCALL_LIST(SYSCALL_ENUM_ENTRY) __SYSCALL_COUNT
};

static Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4, uintptr_t p5)
{
    Result __ret = ERR_NOT_IMPLEMENTED;

#if defined(__x86_64__)

    asm volatile("push %%rbx; movq %2,%%rbx; push %%rbp; int $0x80; pop %%rbp; pop %%rbx"
                 : "=a"(__ret)
                 : "a"(syscall), "r"(p1), "c"(p2), "d"(p3), "S"(p4), "D"(p5)
                 : "memory");

#elif defined(__i386__)
    // ebp need to be saved because hj_process_clone will trash it.
    asm volatile("push %%ebx; movl %2,%%ebx; push %%ebp; int $0x80; pop %%ebp; pop %%ebx"
                 : "=a"(__ret)
                 : "0"(syscall), "r"(p1), "c"(p2), "d"(p3), "S"(p4), "D"(p5)
                 : "memory");
#endif

    return __ret;
}

#ifdef __cplusplus

static inline Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2, uintptr_t p3, uintptr_t p4)
{
    return __syscall(syscall, p1, p2, p3, p4, 0);
}

static inline Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2, uintptr_t p3)
{
    return __syscall(syscall, p1, p2, p3, 0, 0);
}

static inline Result __syscall(Syscall syscall, uintptr_t p1, uintptr_t p2)
{
    return __syscall(syscall, p1, p2, 0, 0, 0);
}

static inline Result __syscall(Syscall syscall, uintptr_t p1)
{
    return __syscall(syscall, p1, 0, 0, 0, 0);
}

static inline Result __syscall(Syscall syscall)
{
    return __syscall(syscall, 0, 0, 0, 0, 0);
}

#endif

__BEGIN_HEADER

Result hj_process_this(int *pid);
Result hj_process_name(char *name, size_t size);
Result hj_process_launch(Launchpad *launchpad, int *pid);
Result hj_process_clone(int *pid, TaskFlags flags);
Result hj_process_exec(Launchpad *launchpad);
Result hj_process_exit(int exit_code);
Result hj_process_cancel(int pid);
Result hj_process_sleep(int time);
Result hj_process_wait(int tid, int *user_exit_value);

Result hj_memory_alloc(size_t size, uintptr_t *out_address);
Result hj_memory_map(uintptr_t address, size_t size, int flags);
Result hj_memory_free(uintptr_t address);
Result hj_memory_include(int handle, uintptr_t *out_address, size_t *out_size);
Result hj_memory_get_handle(uintptr_t address, int *out_handle);

Result hj_filesystem_mkdir(const char *raw_path, size_t size);
Result hj_filesystem_mkpipe(const char *raw_path, size_t size);
Result hj_filesystem_link(const char *raw_old_path, size_t old_size, const char *raw_new_path, size_t new_size);
Result hj_filesystem_unlink(const char *raw_path, size_t size);
Result hj_filesystem_rename(const char *raw_old_path, size_t old_size, const char *raw_new_path, size_t new_size);

Result hj_system_info(SystemInfo *info);
Result hj_system_status(SystemStatus *status);
Result hj_system_time(TimeStamp *timestamp);
Result hj_system_tick(uint32_t *tick);
Result hj_system_reboot();
Result hj_system_shutdown();

Result hj_create_pipe(int *reader_handle, int *writer_handle);
Result hj_create_term(int *server_handle, int *client_handle);

Result hj_handle_open(int *handle, const char *raw_path, size_t size, OpenFlag flags);
Result hj_handle_close(int handle);
Result hj_handle_reopen(int handle, int *reopened);
Result hj_handle_copy(int source, int destination);
Result hj_handle_poll(HandlePoll *handles, size_t count, Timeout timeout);
Result hj_handle_read(int handle, void *buffer, size_t size, size_t *read);
Result hj_handle_write(int handle, const void *buffer, size_t size, size_t *written);
Result hj_handle_call(int handle, IOCall request, void *args);
Result hj_handle_seek(int handle, ssize64_t *offset, HjWhence whence, ssize64_t *result);
Result hj_handle_stat(int handle, FileState *state);
Result hj_handle_connect(int *handle, const char *raw_path, size_t size);
Result hj_handle_accept(int handle, int *connection_handle);

__END_HEADER
