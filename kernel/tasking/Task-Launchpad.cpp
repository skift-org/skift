#include <libfile/ELF32.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task-Memory.h"
#include "kernel/tasking/Task.h"

Result task_launch_load_elf(Task *parent_task, Task *child_task, Stream *elf_file, ELF32Program *program_header)
{
    if (program_header->vaddr <= 0x100000)
    {
        logger_error("ELF program no in user memory (0x%08x)!", program_header->vaddr);
        return ERR_EXEC_FORMAT_ERROR;
    }

    void *parent_address_space = task_switch_address_space(parent_task, child_task->address_space);

    MemoryRange range = MemoryRange::around_non_aligned_address(program_header->vaddr, program_header->memsz);

    task_memory_map(child_task, range.base(), range.size(), MEMORY_CLEAR);

    stream_seek(elf_file, program_header->offset, WHENCE_START);
    size_t read = stream_read(elf_file, (void *)program_header->vaddr, program_header->filesz);

    if (read != program_header->filesz)
    {
        logger_error("Didn't read the right amount from the ELF file!");

        task_switch_address_space(parent_task, parent_address_space);

        return ERR_EXEC_FORMAT_ERROR;
    }
    else
    {
        task_switch_address_space(parent_task, parent_address_space);

        return SUCCESS;
    }
}

void task_launch_passhandle(Task *parent_task, Task *child_task, Launchpad *launchpad)
{
    LockHolder holder(parent_task->handles_lock);

    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        int child_handle_id = i;
        int parent_handle_id = launchpad->handles[i];

        if (parent_handle_id >= 0 &&
            parent_handle_id < PROCESS_HANDLE_COUNT &&
            parent_task->handles[parent_handle_id] != nullptr)
        {
            fshandle_acquire_lock(parent_task->handles[parent_handle_id], scheduler_running_id());
            child_task->handles[child_handle_id] = fshandle_clone(parent_task->handles[parent_handle_id]);
            fshandle_release_lock(parent_task->handles[parent_handle_id], scheduler_running_id());
        }
    }
}

Result task_launch(Task *parent_task, Launchpad *launchpad, int *pid)
{
    assert(parent_task == scheduler_running());

    *pid = -1;

    __cleanup(stream_cleanup) Stream *elf_file = stream_open(launchpad->executable, OPEN_READ);

    if (handle_has_error(elf_file))
    {
        logger_error("Failed to open ELF file %s: %s!", launchpad->executable, handle_error_string(elf_file));
        return handle_get_error(elf_file);
    }

    ELF32Header elf_header;
    {
        size_t elf_header_size = stream_read(elf_file, &elf_header, sizeof(ELF32Header));

        if (elf_header_size != sizeof(ELF32Header) || !elf_header.valid())
        {
            logger_error("Failed to load ELF file %s: bad exec format!", launchpad->executable);
            return ERR_EXEC_FORMAT_ERROR;
        }
    }

    {
        Task *child_task = task_spawn_with_argv(parent_task, launchpad->name, (TaskEntryPoint)elf_header.entry, (const char **)launchpad->argv, true);

        for (int i = 0; i < elf_header.phnum; i++)
        {
            ELF32Program elf_program_header;
            stream_seek(elf_file, elf_header.phoff + elf_header.phentsize * i, WHENCE_START);

            if (stream_read(elf_file, &elf_program_header, sizeof(ELF32Program)) != sizeof(ELF32Program))
            {
                task_destroy(child_task);
                return ERR_EXEC_FORMAT_ERROR;
            }

            Result result = task_launch_load_elf(parent_task, child_task, elf_file, &elf_program_header);

            if (result != SUCCESS)
            {
                task_destroy(child_task);
                return result;
            }
        }

        task_launch_passhandle(parent_task, child_task, launchpad);

        *pid = child_task->id;
        task_go(child_task);
    }

    return SUCCESS;
}
