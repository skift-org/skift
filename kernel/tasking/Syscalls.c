
/* syscalls.c syscalls handeling code                                         */

#include <libsystem/Assert.h>
#include <libsystem/Atomic.h>
#include <libsystem/BuildInfo.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/Result.h>

#include "arch/Arch.h"
#include "kernel/filesystem/Filesystem.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/system/System.h"
#include "kernel/tasking.h"
#include "kernel/tasking/Handles.h"
#include "kernel/tasking/Syscalls.h"

typedef Result (*SyscallHandler)(int, int, int, int, int);

bool syscall_validate_ptr(uintptr_t ptr, size_t size)
{
    return ptr >= 0x100000 &&
           ptr + size >= 0x100000 &&
           ptr + size >= ptr;
}

/* --- Process -------------------------------------------------------------- */

Result sys_process_this(int *pid)
{
    if (!syscall_validate_ptr((uintptr_t)pid, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    *pid = scheduler_running_id();

    return SUCCESS;
}

Result sys_process_launch(Launchpad *launchpad, int *pid)
{
    if (!syscall_validate_ptr((uintptr_t)launchpad, sizeof(Launchpad)) ||
        !syscall_validate_ptr((uintptr_t)pid, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_launch(scheduler_running(), launchpad, pid);
}

Result sys_process_exit(int code)
{
    task_exit(code);

    ASSERT_NOT_REACHED();
}

Result sys_process_cancel(int pid)
{
    Result result = SUCCESS;

    ATOMIC({
        Task *task = task_by_id(pid);

        if (task == NULL)
        {
            result = ERR_NO_SUCH_TASK;
        }
        else
        {
            result = task_cancel(task, -1);
        }
    });

    return result;
}

Result sys_process_map(uintptr_t address, size_t size)
{
    if (syscall_validate_ptr(address, size))
    {
        return ERR_BAD_ADDRESS;
    }

    if (address % PAGE_SIZE || size % PAGE_SIZE)
    {
        return ERR_MEMORY_NOT_ALIGNED;
    }

    return task_memory_map(scheduler_running(), (MemoryRange){address, size});
}

Result sys_process_alloc(uintptr_t size, uintptr_t *out_address)
{
    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_memory_alloc(scheduler_running(), size, out_address);
}

Result sys_process_free(uint address, uint size)
{
    if (!syscall_validate_ptr(address, size))
    {
        return ERR_BAD_ADDRESS;
    }

    if (address % PAGE_SIZE || size % PAGE_SIZE)
    {
        return ERR_MEMORY_NOT_ALIGNED;
    }

    return task_memory_free(scheduler_running(), (MemoryRange){address, size});
}

Result sys_process_get_directory(char *buffer, uint size)
{
    return task_get_directory(scheduler_running(), buffer, size);
}

Result sys_process_set_directory(const char *path)
{
    return task_set_directory(scheduler_running(), path);
}

Result sys_process_sleep(int time)
{
    return task_sleep(scheduler_running(), time);
}

Result sys_process_wait(int tid, int *user_exit_value)
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

Result sys_shared_memory_alloc(size_t size, uintptr_t *out_address)
{
    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_shared_memory_alloc(scheduler_running(), size, out_address);
}

Result sys_shared_memory_free(uintptr_t address)
{

    return task_shared_memory_free(scheduler_running(), address);
}

Result sys_shared_memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    if (!syscall_validate_ptr((uintptr_t)out_address, sizeof(uintptr_t)) ||
        !syscall_validate_ptr((uintptr_t)out_size, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_shared_memory_include(scheduler_running(), handle, out_address, out_size);
}

Result sys_shared_memory_get_handle(uintptr_t address, int *out_handle)
{
    if (!syscall_validate_ptr((uintptr_t)out_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_shared_memory_get_handle(scheduler_running(), address, out_handle);
}

/* --- Filesystem ----------------------------------------------------------- */

Result sys_filesystem_mkdir(const char *dir_path)
{
    Path *path = task_resolve_directory(scheduler_running(), dir_path);

    Result result = filesystem_mkdir(path);

    path_destroy(path);

    return result;
}

Result sys_filesystem_mkpipe(const char *fifo_path)
{
    Path *path = task_resolve_directory(scheduler_running(), fifo_path);

    Result result = filesystem_mkpipe(path);

    path_destroy(path);

    return result;
}

Result sys_filesystem_link(const char *old_path, const char *new_path)
{
    Path *oldp = task_resolve_directory(scheduler_running(), old_path);
    Path *newp = task_resolve_directory(scheduler_running(), new_path);

    Result result = filesystem_mklink(oldp, newp);

    path_destroy(oldp);
    path_destroy(newp);

    return result;
}

Result sys_filesystem_unlink(const char *link_path)
{
    Path *path = task_resolve_directory(scheduler_running(), link_path);

    Result result = filesystem_unlink(path);

    path_destroy(path);

    return result;
}

Result sys_filesystem_rename(const char *old_path, const char *new_path)
{
    Path *oldp = task_resolve_directory(scheduler_running(), old_path);
    Path *newp = task_resolve_directory(scheduler_running(), new_path);

    Result result = filesystem_rename(oldp, newp);

    path_destroy(oldp);
    path_destroy(newp);

    return result;
}

/* --- System info getter --------------------------------------------------- */

Result sys_system_get_info(SystemInfo *info)
{
    strncpy(info->kernel_name, "hjert", SYSTEM_INFO_FIELD_SIZE);

    strncpy(info->kernel_release, __BUILD_GITREF__, SYSTEM_INFO_FIELD_SIZE);

    strlcpy(info->system_name, "skift", SYSTEM_INFO_FIELD_SIZE);

    // FIXME: this should not be hard coded.
    strlcpy(info->machine, "machine", SYSTEM_INFO_FIELD_SIZE);

    return SUCCESS;
}

ElapsedTime system_get_uptime(void);

Result sys_system_get_status(SystemStatus *status)
{
    // FIXME: get a real uptime value;
    status->uptime = system_get_uptime();

    status->total_ram = memory_get_total();
    status->used_ram = memory_get_used();

    status->running_tasks = task_count();
    status->cpu_usage = 100 - scheduler_get_usage(0);

    return SUCCESS;
}

Result sys_system_get_time(TimeStamp *timestamp)
{
    *timestamp = arch_get_time();

    return SUCCESS;
}

Result sys_system_get_ticks(uint32_t *tick)
{
    if (!syscall_validate_ptr((uintptr_t)tick, sizeof(uintptr_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    *tick = system_get_tick();
    return SUCCESS;
}

/* --- Create --------------------------------------------------------------- */

Result sys_create_pipe(int *reader_handle, int *writer_handle)
{
    if (!syscall_validate_ptr((uintptr_t)reader_handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)writer_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_create_pipe(scheduler_running(), reader_handle, writer_handle);
}

Result sys_create_term(int *master_handle, int *slave_handle)
{
    if (!syscall_validate_ptr((uintptr_t)master_handle, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)slave_handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_create_term(scheduler_running(), master_handle, slave_handle);
}

/* --- Handles -------------------------------------------------------------- */

Result sys_handle_open(int *handle, const char *path, OpenFlag flags)
{
    if (!syscall_validate_ptr((uintptr_t)handle, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_open(scheduler_running(), handle, path, flags);
}

Result sys_handle_close(int handle)
{
    return task_fshandle_close(scheduler_running(), handle);
}

Result sys_handle_select(
    HandleSet *handles_set,
    int *selected,
    SelectEvent *selected_events,
    Timeout timeout)
{
    if (!syscall_validate_ptr((uintptr_t)handles_set, sizeof(HandleSet)) ||
        !syscall_validate_ptr((uintptr_t)handles_set->handles, sizeof(int) * handles_set->count) ||
        !syscall_validate_ptr((uintptr_t)handles_set->events, sizeof(SelectEvent) * handles_set->count) ||
        !syscall_validate_ptr((uintptr_t)selected, sizeof(int)) ||
        !syscall_validate_ptr((uintptr_t)selected_events, sizeof(SelectEvent)))
    {
        return ERR_BAD_ADDRESS;
    }

    if (handles_set->count > PROCESS_HANDLE_COUNT)
    {
        return ERR_TOO_MANY_OPEN_FILES;
    }

    // We need to copy these because this syscall uses task_fshandle_select
    // who block the current thread using a blocker which does a context switch.

    __cleanup_malloc int *handles_copy = (int *)calloc(handles_set->count, sizeof(int));
    memcpy(handles_copy, handles_set->handles, handles_set->count * sizeof(int));

    __cleanup_malloc SelectEvent *events_copy = (SelectEvent *)calloc(handles_set->count, sizeof(SelectEvent));
    memcpy(events_copy, handles_set->events, handles_set->count * sizeof(SelectEvent));

    int selected_copy;
    SelectEvent selected_event_copy;

    HandleSet handle_set = (HandleSet){handles_copy, events_copy, handles_set->count};

    Result result = task_fshandle_select(
        scheduler_running(),
        &handle_set,
        &selected_copy,
        &selected_event_copy,
        timeout);

    *selected = selected_copy;
    *selected_events = selected_event_copy;

    return result;
}

Result sys_handle_read(int handle, char *buffer, size_t size, size_t *read)
{
    if (!syscall_validate_ptr((uintptr_t)buffer, size) ||
        !syscall_validate_ptr((uintptr_t)read, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_read(scheduler_running(), handle, buffer, size, read);
}

Result sys_handle_write(int handle, const char *buffer, size_t size, size_t *written)
{
    if (!syscall_validate_ptr((uintptr_t)buffer, size) ||
        !syscall_validate_ptr((uintptr_t)written, sizeof(size_t)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_write(scheduler_running(), handle, buffer, size, written);
}

Result sys_handle_call(int handle, IOCall request, void *args)
{
    return task_fshandle_call(scheduler_running(), handle, request, args);
}

Result sys_handle_seek(int handle, int offset, Whence whence)
{
    return task_fshandle_seek(scheduler_running(), handle, offset, whence);
}

Result sys_handle_tell(int handle, Whence whence, int *offset)
{
    if (!syscall_validate_ptr((uintptr_t)offset, sizeof(int)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_tell(scheduler_running(), handle, whence, offset);
}

Result sys_handle_stat(int handle, FileState *state)
{
    if (!syscall_validate_ptr((uintptr_t)state, sizeof(FileState)))
    {
        return ERR_BAD_ADDRESS;
    }

    return task_fshandle_stat(scheduler_running(), handle, state);
}

Result sys_handle_connect(int *handle, const char *path)
{
    return task_fshandle_connect(scheduler_running(), handle, path);
}

Result sys_handle_accept(int handle, int *connection_handle)
{
    return task_fshandle_accept(scheduler_running(), handle, connection_handle);
}

static Result (*syscalls[__SYSCALL_COUNT])() = {
    [SYS_PROCESS_THIS] = sys_process_this,
    [SYS_PROCESS_LAUNCH] = sys_process_launch,
    [SYS_PROCESS_EXIT] = sys_process_exit,
    [SYS_PROCESS_CANCEL] = sys_process_cancel,
    [SYS_PROCESS_SLEEP] = sys_process_sleep,
    [SYS_PROCESS_WAIT] = sys_process_wait,
    [SYS_PROCESS_GET_DIRECTORY] = sys_process_get_directory,
    [SYS_PROCESS_SET_DIRECTORY] = sys_process_set_directory,
    [SYS_PROCESS_MAP] = sys_process_map,
    [SYS_PROCESS_ALLOC] = sys_process_alloc,
    [SYS_PROCESS_FREE] = sys_process_free,

    [SYS_SHARED_MEMORY_ALLOC] = sys_shared_memory_alloc,
    [SYS_SHARED_MEMORY_FREE] = sys_shared_memory_free,
    [SYS_SHARED_MEMORY_INCLUDE] = sys_shared_memory_include,
    [SYS_SHARED_MEMORY_GET_HANDLE] = sys_shared_memory_get_handle,

    [SYS_FILESYSTEM_MKDIR] = sys_filesystem_mkdir,
    [SYS_FILESYSTEM_MKPIPE] = sys_filesystem_mkpipe,
    [SYS_FILESYSTEM_LINK] = sys_filesystem_link,
    [SYS_FILESYSTEM_UNLINK] = sys_filesystem_unlink,
    [SYS_FILESYSTEM_RENAME] = sys_filesystem_rename,

    [SYS_SYSTEM_GET_INFO] = sys_system_get_info,
    [SYS_SYSTEM_GET_STATUS] = sys_system_get_status,
    [SYS_SYSTEM_GET_TIME] = sys_system_get_time,
    [SYS_SYSTEM_GET_TICKS] = sys_system_get_ticks,

    [SYS_CREATE_PIPE] = sys_create_pipe,
    [SYS_CREATE_TERM] = sys_create_term,

    [SYS_HANDLE_OPEN] = sys_handle_open,
    [SYS_HANDLE_CLOSE] = sys_handle_close,
    [SYS_HANDLE_SELECT] = sys_handle_select,
    [SYS_HANDLE_READ] = sys_handle_read,
    [SYS_HANDLE_WRITE] = sys_handle_write,
    [SYS_HANDLE_CALL] = sys_handle_call,
    [SYS_HANDLE_SEEK] = sys_handle_seek,
    [SYS_HANDLE_TELL] = sys_handle_tell,
    [SYS_HANDLE_STAT] = sys_handle_stat,
    [SYS_HANDLE_CONNECT] = sys_handle_connect,
    [SYS_HANDLE_ACCEPT] = sys_handle_accept,
};

SyscallHandler syscall_get_handler(Syscall syscall)
{
    if (syscall >= 0 && syscall < __SYSCALL_COUNT)
    {
        if ((SyscallHandler)syscalls[syscall] == NULL)
        {
            logger_error("Syscall not implemented ID=%d call by PROCESS=%d.", syscall, scheduler_running_id());
        }

        return (SyscallHandler)syscalls[syscall];
    }
    else
    {
        logger_error("Unknow syscall ID=%d call by PROCESS=%d.", syscall, scheduler_running_id());
        return NULL;
    }
}

#define SYSCALL_NAMES_ENTRY(__entry) #__entry,

static const char *syscall_names[] = {SYSCALL_LIST(SYSCALL_NAMES_ENTRY)};

int task_do_syscall(Syscall syscall, int arg0, int arg1, int arg2, int arg3, int arg4)
{
    SyscallHandler handler = syscall_get_handler(syscall);

    Result result = SUCCESS;

    if (handler == NULL)
    {
        logger_error("Invalid syscall: %d", syscall);
        return ERR_FUNCTION_NOT_IMPLEMENTED;
    }

    result = handler(arg0, arg1, arg2, arg3, arg4);

    if (result != SUCCESS && result != TIMEOUT)
    {
        logger_trace("%s(%08x, %08x, %08x, %08x, %08x) returned %s", syscall_names[syscall], arg0, arg1, arg2, arg3, arg4, result_to_string((Result)result));
    }

    return result;
}
