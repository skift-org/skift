#include <assert.h>
#include <string.h>

#include "system/Streams.h"
#include <abi/Result.h>
#include <libsystem/BuildInfo.h>

#include "archs/Arch.h"

#include "system/interrupts/Interupts.h"
#include "system/scheduling/Scheduler.h"
#include "system/system/System.h"
#include "system/tasking/Syscalls.h"
#include "system/tasking/Task-Launchpad.h"
#include "system/tasking/Task-Memory.h"

typedef HjResult (*SyscallHandler)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);

bool syscall_validate_ptr(uintptr_t ptr, size_t size)
{
    return ptr >= 0x100000 && ptr + size >= 0x100000 && ptr + size >= ptr;
}

/* --- Process -------------------------------------------------------------- */

HjResult hj_process_this(int *pid)
{
    if (!syscall_validate_ptr((uintptr_t)pid, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    *pid = scheduler_running_id();

    return SUCCESS;
}

HjResult hj_process_name(char *name, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)name, size))
    {
        return ERR_BAD_ADDRESS;
    }

    strlcpy(name, scheduler_running()->name, size);

    return SUCCESS;
}

static bool validate_launchpad_arguments(Launchpad *launchpad)
{
    for (int i = 0; i < launchpad->argc; i++)
    {
        auto &arg = launchpad->argv[i];

        if (!syscall_validate_ptr((uintptr_t)arg.buffer, arg.size))
        {
            return false;
        }
    }

    return true;
}

static bool valid_launchpad(Launchpad *launchpad)
{
    return syscall_validate_ptr((uintptr_t)launchpad, sizeof(Launchpad)) &&
           validate_launchpad_arguments(launchpad) &&
           syscall_validate_ptr((uintptr_t)launchpad->env, launchpad->env_size);
}

static Launchpad copy_launchpad(Launchpad *launchpad)
{
    Launchpad launchpad_copy = *launchpad;

    for (int i = 0; i < launchpad->argc; i++)
    {
        launchpad_copy.argv[i].buffer = strdup(launchpad->argv[i].buffer);
        launchpad_copy.argv[i].size = launchpad->argv[i].size;
    }

    launchpad_copy.env = strdup(launchpad->env);
    launchpad_copy.env_size = launchpad->env_size;

    return launchpad_copy;
}

static void free_launchpad(Launchpad *launchpad)
{
    free(launchpad->env);

    for (int i = 0; i < launchpad->argc; i++)
    {
        free(launchpad->argv[i].buffer);
    }
}

HjResult hj_process_launch(Launchpad *launchpad, int *pid)
{
    if (!valid_launchpad(launchpad) ||
        !syscall_validate_ptr((uintptr_t)pid, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto launchpad_copy = copy_launchpad(launchpad);

    launchpad_copy.flags |= TASK_USER;

    HjResult result = task_launch(scheduler_running(), &launchpad_copy, pid);

    free_launchpad(&launchpad_copy);

    return result;
}

HjResult hj_process_clone(int *, TaskFlags)
{
    // Implemented in archs/x86_32/Interrupts.cpp
    return ERR_NOT_IMPLEMENTED;
}

HjResult hj_process_exec(Launchpad *launchpad)
{
    if (!valid_launchpad(launchpad))
    {
        return ERR_BAD_ADDRESS;
    }

    auto launchpad_copy = copy_launchpad(launchpad);

    HjResult result = task_exec(scheduler_running(), &launchpad_copy);

    free_launchpad(&launchpad_copy);

    return result;
}

HjResult hj_process_exit(int exit_code)
{
    if (exit_code != PROCESS_SUCCESS)
    {
        Kernel::logln("Process terminated with error code {}!", exit_code);
        Arch::backtrace();
    }

    return scheduler_running()->cancel(exit_code);
}

HjResult hj_process_cancel(int pid)
{
    InterruptsRetainer retainer;

    Task *task = task_by_id(pid);

    if (task == nullptr)
    {
        return ERR_NO_SUCH_TASK;
    }
    else if (!(task->_flags & TASK_USER))
    {
        return ERR_ACCESS_DENIED;
    }
    else
    {
        return task->cancel(PROCESS_FAILURE);
    }
}

HjResult hj_process_sleep(int time)
{
    return task_sleep(scheduler_running(), time);
}

HjResult hj_process_wait(int tid, int *user_exit_value)
{
    int exit_value;

    HjResult result = task_wait(tid, &exit_value);

    if (syscall_validate_ptr((uintptr_t)user_exit_value, sizeof(int)))
    {
        *user_exit_value = exit_value;
    }

    return result;
}

/* --- Shared memory -------------------------------------------------------- */

HjResult hj_memory_alloc(size_t size, uintptr_t *out_address)
{
    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_alloc(scheduler_running(), size, out_address);
}

HjResult hj_memory_map(uintptr_t address, size_t size, int flags)
{
    if (!syscall_validate_ptr(address, size))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_map(scheduler_running(), address, size, flags);
}

HjResult hj_memory_free(uintptr_t address)
{
    return task_memory_free(scheduler_running(), address);
}

HjResult hj_memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{

    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)) ||
        !syscall_validate_ptr((uintptr_t)out_size, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_include(scheduler_running(), handle, out_address, out_size);
}

HjResult hj_memory_get_handle(uintptr_t address, int *out_handle)
{
    if (!syscall_validate_ptr((uintptr_t)out_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_get_handle(scheduler_running(), address, out_handle);
}

/* --- Filesystem ----------------------------------------------------------- */

HjResult hj_filesystem_mkdir(const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = IO::Path::parse(raw_path, size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.mkdir(path);
}

HjResult hj_filesystem_mkpipe(const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = IO::Path::parse(raw_path, size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.mkpipe(path);
}

HjResult hj_filesystem_link(const char *raw_old_path, size_t old_size,
                            const char *raw_new_path, size_t new_size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_old_path, old_size) &&
        !syscall_validate_ptr((uintptr_t)raw_new_path, new_size))
    {
        return ERR_BAD_ADDRESS;
    }

    IO::Path old_path = IO::Path::parse(raw_old_path, old_size).normalized();

    IO::Path new_path = IO::Path::parse(raw_new_path, new_size).normalized();

    auto &domain = scheduler_running()->domain();

    HjResult result = domain.mklink(old_path, new_path);

    return result;
}

HjResult hj_filesystem_unlink(const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = IO::Path::parse(raw_path, size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.unlink(path);
}

HjResult hj_filesystem_rename(const char *raw_old_path, size_t old_size,
                              const char *raw_new_path, size_t new_size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_old_path, old_size) &&
        !syscall_validate_ptr((uintptr_t)raw_new_path, new_size))
    {
        return ERR_BAD_ADDRESS;
    }

    IO::Path old_path = IO::Path::parse(raw_old_path, old_size).normalized();

    IO::Path new_path = IO::Path::parse(raw_new_path, new_size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.rename(old_path, new_path);
}

/* --- System info getter --------------------------------------------------- */

HjResult hj_system_info(SystemInfo *info)
{
    strncpy(info->kernel_name, "hjert", SYSTEM_INFO_FIELD_SIZE);

    strncpy(info->kernel_release, __BUILD_VERSION__, SYSTEM_INFO_FIELD_SIZE);

    strncpy(info->kernel_build, __BUILD_GITREF__, SYSTEM_INFO_FIELD_SIZE);

    strlcpy(info->system_name, "skift", SYSTEM_INFO_FIELD_SIZE);

    // FIXME: this should not be hard coded.
    strlcpy(info->machine, "machine", SYSTEM_INFO_FIELD_SIZE);

    return SUCCESS;
}

ElapsedTime system_get_uptime();

HjResult hj_system_status(SystemStatus *status)
{
    // FIXME: get a real uptime value;
    status->uptime = system_get_uptime();

    status->total_ram = memory_get_total();
    status->used_ram = memory_get_used();

    status->running_tasks = task_count();
    status->cpu_usage = 100 - scheduler_get_usage(0);

    return SUCCESS;
}

HjResult hj_system_get_time(TimeStamp *timestamp)
{
    *timestamp = Arch::get_time();

    return SUCCESS;
}

HjResult hj_system_get_ticks(uint32_t *tick)
{
    if (!syscall_validate_ptr((uintptr_t)tick, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    *tick = system_get_tick();
    return SUCCESS;
}

HjResult hj_system_reboot()
{
    Arch::reboot();
    ASSERT_NOT_REACHED();
}

HjResult hj_system_shutdown()
{
    Arch::shutdown();
    ASSERT_NOT_REACHED();
}

/* --- Create --------------------------------------------------------------- */

HjResult hj_create_pipe(int *reader_handle, int *writer_handle)
{
    if (!syscall_validate_ptr((uintptr_t)reader_handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)writer_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return scheduler_running()->handles().pipe(reader_handle, writer_handle);
}

HjResult hj_create_term(int *server_handle, int *client_handle)
{
    if (!syscall_validate_ptr((uintptr_t)server_handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)client_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    return handles.term(server_handle, client_handle);
}

/* --- Handles -------------------------------------------------------------- */

HjResult hj_handle_open(int *handle,
                        const char *raw_path, size_t size,
                        HjOpenFlag flags)
{
    if (!syscall_validate_ptr((uintptr_t)handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = IO::Path::parse(raw_path, size).normalized();

    auto &handles = scheduler_running()->handles();

    auto &domain = scheduler_running()->domain();

    auto result_or_handle_index = handles.open(domain, path, flags);

    if (result_or_handle_index.success())
    {
        *handle = result_or_handle_index.unwrap();
        return SUCCESS;
    }
    else
    {
        *handle = HANDLE_INVALID_ID;
        return result_or_handle_index.result();
    }
}

HjResult hj_handle_close(int handle)
{
    auto &handles = scheduler_running()->handles();

    return handles.close(handle);
}

HjResult hj_handle_reopen(int handle, int *reopened)
{
    if (!syscall_validate_ptr((uintptr_t)reopened, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    return handles.reopen(handle, reopened);
}

HjResult hj_handle_copy(int source, int destination)
{
    auto &handles = scheduler_running()->handles();

    return handles.copy(source, destination);
}

HjResult hj_handle_poll(HandlePoll *handle_poll, size_t count, Timeout timeout)
{
    if (!syscall_validate_ptr((uintptr_t)handle_poll, sizeof(HandlePoll) * count))
    {
        return ERR_BAD_ADDRESS;
    }

    if (count > PROCESS_HANDLE_COUNT)
    {
        return ERR_TOO_MANY_HANDLE;
    }

    if (count == 0)
    {
        return SUCCESS;
    }

    for (size_t i = 0; i < count; i++)
    {
        handle_poll[i].result = 0;
    }

    auto &handles = scheduler_running()->handles();

    return handles.poll(handle_poll, count, timeout);
}

HjResult hj_handle_read(int handle, void *buffer, size_t size, size_t *read)
{
    if (!syscall_validate_ptr((uintptr_t)buffer, size) ||
        !syscall_validate_ptr((uintptr_t)read, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    auto result_or_read = handles.read(handle, buffer, size);

    if (result_or_read.success())
    {
        *read = result_or_read.unwrap();
        return SUCCESS;
    }
    else
    {
        *read = 0;
        return result_or_read.result();
    }
}

HjResult hj_handle_write(int handle, const void *buffer, size_t size, size_t *written)
{
    if (!syscall_validate_ptr((uintptr_t)buffer, size) ||
        !syscall_validate_ptr((uintptr_t)written, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    auto result_or_written = handles.write(handle, buffer, size);

    if (result_or_written.success())
    {
        *written = result_or_written.unwrap();
        return SUCCESS;
    }
    else
    {
        *written = 0;
        return result_or_written.result();
    }
}

HjResult hj_handle_call(int handle, IOCall request, void *args)
{
    auto &handles = scheduler_running()->handles();

    return handles.call(handle, request, args);
}

HjResult hj_handle_seek(int handle, ssize64_t *offset_ptr, HjWhence whence, ssize64_t *result_ptr)
{
    if ((offset_ptr != nullptr && !syscall_validate_ptr((uintptr_t)offset_ptr, sizeof(int))) ||
        (result_ptr != nullptr && !syscall_validate_ptr((uintptr_t)result_ptr, sizeof(int))))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    ssize64_t offset = offset_ptr != nullptr ? *offset_ptr : 0;

    auto seek_result = handles.seek(handle, {(IO::Whence)whence, offset});

    if (result_ptr != nullptr)
    {
        *result_ptr = seek_result.unwrap_or(0);
    }

    return seek_result.result();
}

HjResult hj_handle_stat(int handle, HjStat *state)
{
    if (!syscall_validate_ptr((uintptr_t)state, sizeof(HjStat)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    return handles.stat(handle, state);
}

HjResult hj_handle_connect(int *handle, const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)handle, sizeof(int)) &&
        !syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = IO::Path::parse(raw_path, size).normalized();

    auto &handles = scheduler_running()->handles();

    auto &domain = scheduler_running()->domain();

    auto result_or_handle_index = handles.connect(domain, path);

    if (result_or_handle_index.success())
    {
        *handle = result_or_handle_index.unwrap();
        return SUCCESS;
    }
    else
    {
        *handle = ERR_INVALID_ARGUMENT;
        return result_or_handle_index.result();
    }
}

HjResult hj_handle_accept(int handle, int *connection_handle)
{
    if (!syscall_validate_ptr((uintptr_t)connection_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    auto result_or_handle_index = handles.accept(handle);

    if (result_or_handle_index.success())
    {
        *connection_handle = result_or_handle_index.unwrap();
        return SUCCESS;
    }
    else
    {
        *connection_handle = ERR_INVALID_ARGUMENT;
        return result_or_handle_index.result();
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"

static SyscallHandler syscalls[__SYSCALL_COUNT] = {
    [HJ_PROCESS_THIS] = reinterpret_cast<SyscallHandler>(hj_process_this),
    [HJ_PROCESS_NAME] = reinterpret_cast<SyscallHandler>(hj_process_name),
    [HJ_PROCESS_LAUNCH] = reinterpret_cast<SyscallHandler>(hj_process_launch),
    [HJ_PROCESS_CLONE] = reinterpret_cast<SyscallHandler>(hj_process_clone),
    [HJ_PROCESS_EXEC] = reinterpret_cast<SyscallHandler>(hj_process_exec),
    [HJ_PROCESS_EXIT] = reinterpret_cast<SyscallHandler>(hj_process_exit),
    [HJ_PROCESS_CANCEL] = reinterpret_cast<SyscallHandler>(hj_process_cancel),
    [HJ_PROCESS_SLEEP] = reinterpret_cast<SyscallHandler>(hj_process_sleep),
    [HJ_PROCESS_WAIT] = reinterpret_cast<SyscallHandler>(hj_process_wait),
    [HJ_MEMORY_ALLOC] = reinterpret_cast<SyscallHandler>(hj_memory_alloc),
    [HJ_MEMORY_MAP] = reinterpret_cast<SyscallHandler>(hj_memory_map),
    [HJ_MEMORY_FREE] = reinterpret_cast<SyscallHandler>(hj_memory_free),
    [HJ_MEMORY_INCLUDE] = reinterpret_cast<SyscallHandler>(hj_memory_include),
    [HJ_MEMORY_GET_HANDLE] = reinterpret_cast<SyscallHandler>(hj_memory_get_handle),
    [HJ_FILESYSTEM_LINK] = reinterpret_cast<SyscallHandler>(hj_filesystem_link),
    [HJ_FILESYSTEM_UNLINK] = reinterpret_cast<SyscallHandler>(hj_filesystem_unlink),
    [HJ_FILESYSTEM_RENAME] = reinterpret_cast<SyscallHandler>(hj_filesystem_rename),
    [HJ_FILESYSTEM_MKPIPE] = reinterpret_cast<SyscallHandler>(hj_filesystem_mkpipe),
    [HJ_FILESYSTEM_MKDIR] = reinterpret_cast<SyscallHandler>(hj_filesystem_mkdir),
    [HJ_SYSTEM_INFO] = reinterpret_cast<SyscallHandler>(hj_system_info),
    [HJ_SYSTEM_STATUS] = reinterpret_cast<SyscallHandler>(hj_system_status),
    [HJ_SYSTEM_TIME] = reinterpret_cast<SyscallHandler>(hj_system_get_time),
    [HJ_SYSTEM_TICKS] = reinterpret_cast<SyscallHandler>(hj_system_get_ticks),
    [HJ_SYSTEM_REBOOT] = reinterpret_cast<SyscallHandler>(hj_system_reboot),
    [HJ_SYSTEM_SHUTDOWN] = reinterpret_cast<SyscallHandler>(hj_system_shutdown),
    [HJ_HANDLE_OPEN] = reinterpret_cast<SyscallHandler>(hj_handle_open),
    [HJ_HANDLE_CLOSE] = reinterpret_cast<SyscallHandler>(hj_handle_close),
    [HJ_HANDLE_REOPEN] = reinterpret_cast<SyscallHandler>(hj_handle_reopen),
    [HJ_HANDLE_COPY] = reinterpret_cast<SyscallHandler>(hj_handle_copy),
    [HJ_HANDLE_POLL] = reinterpret_cast<SyscallHandler>(hj_handle_poll),
    [HJ_HANDLE_READ] = reinterpret_cast<SyscallHandler>(hj_handle_read),
    [HJ_HANDLE_WRITE] = reinterpret_cast<SyscallHandler>(hj_handle_write),
    [HJ_HANDLE_CALL] = reinterpret_cast<SyscallHandler>(hj_handle_call),
    [HJ_HANDLE_SEEK] = reinterpret_cast<SyscallHandler>(hj_handle_seek),
    [HJ_HANDLE_STAT] = reinterpret_cast<SyscallHandler>(hj_handle_stat),
    [HJ_HANDLE_CONNECT] = reinterpret_cast<SyscallHandler>(hj_handle_connect),
    [HJ_HANDLE_ACCEPT] = reinterpret_cast<SyscallHandler>(hj_handle_accept),
    [HJ_CREATE_PIPE] = reinterpret_cast<SyscallHandler>(hj_create_pipe),
    [HJ_CREATE_TERM] = reinterpret_cast<SyscallHandler>(hj_create_term),
};

#pragma GCC diagnostic pop

SyscallHandler syscall_get_handler(Syscall syscall)
{
    if (syscall >= 0 && syscall < __SYSCALL_COUNT)
    {
        if ((SyscallHandler)syscalls[syscall] == nullptr)
        {
            Kernel::logln("Syscall not implemented ID={} call by PROCESS={}.", syscall, scheduler_running_id());
        }

        return (SyscallHandler)syscalls[syscall];
    }
    else
    {
        Kernel::logln("Unknow syscall ID={} call by PROCESS={}.", syscall, scheduler_running_id());
        return nullptr;
    }
}

#define SYSCALL_NAMES_ENTRY(__entry) #__entry,
static const char *syscall_names[] = {SYSCALL_LIST(SYSCALL_NAMES_ENTRY)};

uintptr_t task_do_syscall(Syscall syscall, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4)
{
    SyscallHandler handler = syscall_get_handler(syscall);

    HjResult result = SUCCESS;

    if (handler == nullptr)
    {
        Kernel::logln("Invalid syscall: {}", syscall);
        return ERR_INVALID_ARGUMENT;
    }

    scheduler_running()->begin_syscall(syscall);
    result = handler(arg0, arg1, arg2, arg3, arg4);
    scheduler_running()->end_syscall();

    if (result != SUCCESS && result != TIMEOUT)
    {
        Kernel::logln(
            "{}({08x}, {08x}, {08x}, {08x}, {08x}) returned {}",
            syscall_names[syscall],
            arg0, arg1, arg2, arg3, arg4,
            result_to_string((HjResult)result));
    }

    scheduler_running()->kill_me_if_you_dare();

    return result;
}
