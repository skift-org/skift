#include <assert.h>
#include <string.h>

#include <libsystem/BuildInfo.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>

#include "archs/Architectures.h"

#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking/Syscalls.h"
#include "kernel/tasking/Task-Launchpad.h"
#include "kernel/tasking/Task-Memory.h"

typedef Result (*SyscallHandler)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);

bool syscall_validate_ptr(uintptr_t ptr, size_t size)
{
    return ptr >= 0x100000 && ptr + size >= 0x100000 && ptr + size >= ptr;
}

/* --- Process -------------------------------------------------------------- */

Result hj_process_this(int *pid)
{
    if (!syscall_validate_ptr((uintptr_t)pid, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    *pid = scheduler_running_id();

    return SUCCESS;
}

Result hj_process_name(char *name, size_t size)
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

Result hj_process_launch(Launchpad *launchpad, int *pid)
{
    if (!valid_launchpad(launchpad) ||
        !syscall_validate_ptr((uintptr_t)pid, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto launchpad_copy = copy_launchpad(launchpad);

    Result result = task_launch(scheduler_running(), &launchpad_copy, pid);

    free_launchpad(&launchpad_copy);

    return result;
}

Result hj_process_clone(int *pid)
{
    // Implemented in archs/x86_32/kernel/Interrupts.cpp
    __unused(pid);
    ASSERT_NOT_REACHED();
}

Result hj_process_exec(Launchpad *launchpad)
{
    if (!valid_launchpad(launchpad))
    {
        return ERR_BAD_ADDRESS;
    }

    auto launchpad_copy = copy_launchpad(launchpad);

    Result result = task_exec(scheduler_running(), &launchpad_copy);

    free_launchpad(&launchpad_copy);

    return result;
}

Result hj_process_exit(int exit_code)
{
    if (exit_code != PROCESS_SUCCESS)
    {
        logger_error("Process terminated with error code %d!", exit_code);
        arch_backtrace();
    }

    scheduler_running()->cancel(exit_code);
    ASSERT_NOT_REACHED();
}

Result hj_process_cancel(int pid)
{
    InterruptsRetainer retainer;

    Task *task = task_by_id(pid);

    if (task == nullptr)
    {
        return ERR_NO_SUCH_TASK;
    }
    else if (!task->user)
    {
        return ERR_ACCESS_DENIED;
    }
    else
    {
        task->cancel(PROCESS_FAILURE);
        return SUCCESS;
    }
}

Result hj_process_sleep(int time)
{
    return task_sleep(scheduler_running(), time);
}

Result hj_process_wait(int tid, int *user_exit_value)
{
    int exit_value;

    Result result = task_wait(tid, &exit_value);

    if (syscall_validate_ptr((uintptr_t)user_exit_value, sizeof(int)))
    {
        *user_exit_value = exit_value;
    }

    return result;
}

/* --- Shared memory -------------------------------------------------------- */

Result hj_memory_alloc(size_t size, uintptr_t *out_address)
{
    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_alloc(scheduler_running(), size, out_address);
}

Result hj_memory_map(uintptr_t address, size_t size, int flags)
{
    if (!syscall_validate_ptr(address, size))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_map(scheduler_running(), address, size, flags);
}

Result hj_memory_free(uintptr_t address)
{
    return task_memory_free(scheduler_running(), address);
}

Result hj_memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{

    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)) ||
        !syscall_validate_ptr((uintptr_t)out_size, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_include(scheduler_running(), handle, out_address, out_size);
}

Result hj_memory_get_handle(uintptr_t address, int *out_handle)
{
    if (!syscall_validate_ptr((uintptr_t)out_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_get_handle(scheduler_running(), address, out_handle);
}

/* --- Filesystem ----------------------------------------------------------- */

Result hj_filesystem_mkdir(const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = Path::parse(raw_path, size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.mkdir(path);
}

Result hj_filesystem_mkpipe(const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = Path::parse(raw_path, size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.mkpipe(path);
}

Result hj_filesystem_link(const char *raw_old_path, size_t old_size,
                          const char *raw_new_path, size_t new_size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_old_path, old_size) &&
        !syscall_validate_ptr((uintptr_t)raw_new_path, new_size))
    {
        return ERR_BAD_ADDRESS;
    }

    Path old_path = Path::parse(raw_old_path, old_size).normalized();

    Path new_path = Path::parse(raw_new_path, new_size).normalized();

    auto &domain = scheduler_running()->domain();

    Result result = domain.mklink(old_path, new_path);

    return result;
}

Result hj_filesystem_unlink(const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = Path::parse(raw_path, size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.unlink(path);
}

Result hj_filesystem_rename(const char *raw_old_path, size_t old_size,
                            const char *raw_new_path, size_t new_size)
{
    if (!syscall_validate_ptr((uintptr_t)raw_old_path, old_size) &&
        !syscall_validate_ptr((uintptr_t)raw_new_path, new_size))
    {
        return ERR_BAD_ADDRESS;
    }

    Path old_path = Path::parse(raw_old_path, old_size).normalized();

    Path new_path = Path::parse(raw_new_path, new_size).normalized();

    auto &domain = scheduler_running()->domain();

    return domain.rename(old_path, new_path);
}

/* --- System info getter --------------------------------------------------- */

Result hj_system_info(SystemInfo *info)
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

Result hj_system_status(SystemStatus *status)
{
    // FIXME: get a real uptime value;
    status->uptime = system_get_uptime();

    status->total_ram = memory_get_total();
    status->used_ram = memory_get_used();

    status->running_tasks = task_count();
    status->cpu_usage = 100 - scheduler_get_usage(0);

    return SUCCESS;
}

Result hj_system_get_time(TimeStamp *timestamp)
{
    *timestamp = arch_get_time();

    return SUCCESS;
}

Result hj_system_get_ticks(uint32_t *tick)
{
    if (!syscall_validate_ptr((uintptr_t)tick, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    *tick = system_get_tick();
    return SUCCESS;
}

Result hj_system_reboot()
{
    arch_reboot();
    ASSERT_NOT_REACHED();
}

Result hj_system_shutdown()
{
    arch_shutdown();
    ASSERT_NOT_REACHED();
}

/* --- Create --------------------------------------------------------------- */

Result hj_create_pipe(int *reader_handle, int *writer_handle)
{
    if (!syscall_validate_ptr((uintptr_t)reader_handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)writer_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return scheduler_running()->handles().pipe(reader_handle, writer_handle);
}

Result hj_create_term(int *server_handle, int *client_handle)
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

Result hj_handle_open(int *handle,
                      const char *raw_path, size_t size,
                      OpenFlag flags)
{
    if (!syscall_validate_ptr((uintptr_t)handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = Path::parse(raw_path, size).normalized();

    auto &handles = scheduler_running()->handles();

    auto &domain = scheduler_running()->domain();

    auto result_or_handle_index = handles.open(domain, path, flags);

    if (result_or_handle_index.success())
    {
        *handle = result_or_handle_index.take_value();
        return SUCCESS;
    }
    else
    {
        *handle = HANDLE_INVALID_ID;
        return result_or_handle_index.result();
    }
}

Result hj_handle_close(int handle)
{
    auto &handles = scheduler_running()->handles();

    return handles.close(handle);
}

Result hj_handle_reopen(int handle, int *reopened)
{
    if (!syscall_validate_ptr((uintptr_t)reopened, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    return handles.reopen(handle, reopened);
}

Result hj_handle_copy(int source, int destination)
{
    auto &handles = scheduler_running()->handles();

    return handles.copy(source, destination);
}

Result hj_handle_poll(
    HandleSet *handles_set,
    int *selected,
    PollEvent *selected_events,
    Timeout timeout)
{
    if (!syscall_validate_ptr((uintptr_t)handles_set, sizeof(HandleSet)) ||
        !syscall_validate_ptr((uintptr_t)handles_set->handles, sizeof(int) * handles_set->count) ||
        !syscall_validate_ptr((uintptr_t)handles_set->events, sizeof(PollEvent) * handles_set->count) ||
        !syscall_validate_ptr((uintptr_t)selected, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)selected_events, sizeof(PollEvent)))
    {
        return ERR_BAD_ADDRESS;
    }

    if (handles_set->count > PROCESS_HANDLE_COUNT)
    {
        return ERR_TOO_MANY_HANDLE;
    }

    if (handles_set->count == 0)
    {
        *selected = -1;
        *selected_events = 0;

        return SUCCESS;
    }

    // We need to copy these because this syscall uses task_fshandle_poll
    // who block the current thread using a blocker which does a context switch.

    __cleanup_malloc int *handles_copy = (int *)calloc(handles_set->count, sizeof(int));
    memcpy(handles_copy, handles_set->handles, handles_set->count * sizeof(int));

    __cleanup_malloc PollEvent *events_copy = (PollEvent *)calloc(handles_set->count, sizeof(PollEvent));
    memcpy(events_copy, handles_set->events, handles_set->count * sizeof(PollEvent));

    int selected_copy;
    PollEvent selected_event_copy;

    HandleSet handle_set = (HandleSet){handles_copy, events_copy, handles_set->count};

    auto &handles = scheduler_running()->handles();

    Result result = handles.poll(
        &handle_set,
        &selected_copy,
        &selected_event_copy,
        timeout);

    *selected = selected_copy;
    *selected_events = selected_event_copy;

    return result;
}

Result hj_handle_read(int handle, void *buffer, size_t size, size_t *read)
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
        *read = result_or_read.take_value();
        return SUCCESS;
    }
    else
    {
        *read = 0;
        return result_or_read.result();
    }
}

Result hj_handle_write(int handle, const void *buffer, size_t size, size_t *written)
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
        *written = result_or_written.take_value();
        return SUCCESS;
    }
    else
    {
        *written = 0;
        return result_or_written.result();
    }
}

Result hj_handle_call(int handle, IOCall request, void *args)
{
    auto &handles = scheduler_running()->handles();

    return handles.call(handle, request, args);
}

Result hj_handle_seek(int handle, int offset, Whence whence, int *result_offset)
{
    if (result_offset != nullptr &&
        !syscall_validate_ptr((uintptr_t)result_offset, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    auto seek_result = handles.seek(handle, offset, whence);

    if (result_offset != nullptr)
    {
        if (seek_result)
        {
            *result_offset = *seek_result;
        }
        else
        {
            *result_offset = 0;
        }
    }

    return seek_result.result();
}

Result hj_handle_stat(int handle, FileState *state)
{
    if (!syscall_validate_ptr((uintptr_t)state, sizeof(FileState)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    return handles.stat(handle, state);
}

Result hj_handle_connect(int *handle, const char *raw_path, size_t size)
{
    if (!syscall_validate_ptr((uintptr_t)handle, sizeof(int)) &&
        !syscall_validate_ptr((uintptr_t)raw_path, size))
    {
        return ERR_BAD_ADDRESS;
    }

    auto path = Path::parse(raw_path, size).normalized();

    auto &handles = scheduler_running()->handles();

    auto &domain = scheduler_running()->domain();

    auto result_or_handle_index = handles.connect(domain, path);

    if (result_or_handle_index.success())
    {
        *handle = result_or_handle_index.take_value();
        return SUCCESS;
    }
    else
    {
        *handle = ERR_INVALID_ARGUMENT;
        return result_or_handle_index.result();
    }
}

Result hj_handle_accept(int handle, int *connection_handle)
{
    if (!syscall_validate_ptr((uintptr_t)connection_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    auto &handles = scheduler_running()->handles();

    auto result_or_handle_index = handles.accept(handle);

    if (result_or_handle_index.success())
    {
        *connection_handle = result_or_handle_index.take_value();
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
            logger_error("Syscall not implemented ID=%d call by PROCESS=%d.", syscall, scheduler_running_id());
        }

        return (SyscallHandler)syscalls[syscall];
    }
    else
    {
        logger_error("Unknow syscall ID=%d call by PROCESS=%d.", syscall, scheduler_running_id());
        return nullptr;
    }
}

#define SYSCALL_NAMES_ENTRY(__entry) #__entry,
static const char *syscall_names[] = {SYSCALL_LIST(SYSCALL_NAMES_ENTRY)};

uintptr_t task_do_syscall(Syscall syscall, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4)
{
    SyscallHandler handler = syscall_get_handler(syscall);

    Result result = SUCCESS;

    if (handler == nullptr)
    {
        logger_error("Invalid syscall: %d", syscall);
        return ERR_INVALID_ARGUMENT;
    }

    result = handler(arg0, arg1, arg2, arg3, arg4);

    if (result != SUCCESS &&
        result != TIMEOUT &&
        result != ERR_STREAM_CLOSED)
    {
        logger_trace(
            "%s(%08x, %08x, %08x, %08x, %08x) returned %s",
            syscall_names[syscall],
            arg0, arg1, arg2, arg3, arg4,
            result_to_string((Result)result));
    }

    return result;
}
