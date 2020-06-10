#include <libfile/elf.h>
#include <libsystem/Assert.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>

#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking.h"

Result task_launch_load_elf(Task *parent_task, Task *child_task, Stream *elf_file, ELFProgram *program_header)
{
    if (program_header->vaddr <= 0x100000)
    {
        logger_error("ELF program no in user memory (0x%08x)!", program_header->vaddr);
        return ERR_EXEC_FORMAT_ERROR;
    }

    PageDirectory *parent_page_directory = task_switch_pdir(parent_task, child_task->pdir);

    task_memory_map(child_task, memory_range_around_non_aligned_address(program_header->vaddr, program_header->memsz));

    stream_seek(elf_file, program_header->offset, WHENCE_START);
    size_t read = stream_read(elf_file, (void *)program_header->vaddr, program_header->filesz);

    if (read != program_header->filesz)
    {
        logger_error("Didn't read the right amount from the ELF file!");

        task_switch_pdir(parent_task, parent_page_directory);

        return ERR_EXEC_FORMAT_ERROR;
    }
    else
    {
        task_switch_pdir(parent_task, parent_page_directory);

        return SUCCESS;
    }
}

void task_launch_passhandle(Task *parent_task, Task *child_task, Launchpad *launchpad)
{
    lock_acquire(parent_task->handles_lock);

    for (int i = 0; i < PROCESS_HANDLE_COUNT; i++)
    {
        int child_handle_id = i;
        int parent_handle_id = launchpad->handles[i];

        if (parent_handle_id >= 0 &&
            parent_handle_id < PROCESS_HANDLE_COUNT &&
            parent_task->handles[parent_handle_id] != NULL)
        {
            fshandle_acquire_lock(parent_task->handles[parent_handle_id], scheduler_running_id());
            child_task->handles[child_handle_id] = fshandle_clone(parent_task->handles[parent_handle_id]);
            fshandle_release_lock(parent_task->handles[parent_handle_id], scheduler_running_id());
        }
    }

    lock_release(parent_task->handles_lock);
}

Result task_launch(Task *parent_task, Launchpad *launchpad, int *pid)
{
    assert(parent_task == scheduler_running());

    *pid = -1;

    __cleanup(stream_cleanup) Stream *elf_file = stream_open(launchpad->executable, OPEN_READ);

    if (handle_has_error(elf_file))
    {
        logger_error("Failled to open ELF file %s: %s!", launchpad->executable, handle_error_string(elf_file));
        return handle_get_error(elf_file);
    }

    ELFHeader elf_header;
    {
        size_t elf_header_size = stream_read(elf_file, &elf_header, sizeof(ELFHeader));

        if (elf_header_size != sizeof(ELFHeader) || !elf_valid(&elf_header))
        {
            logger_error("Failled to load ELF file %s: bad exec format!", launchpad->executable);
            return ERR_EXEC_FORMAT_ERROR;
        }
    }

    {
        Task *child_task = task_spawn_with_argv(parent_task, launchpad->name, (TaskEntry)elf_header.entry, (const char **)launchpad->argv, true);

        for (int i = 0; i < elf_header.phnum; i++)
        {
            ELFProgram elf_program_header;
            stream_seek(elf_file, elf_header.phoff + (elf_header.phentsize) * i, WHENCE_START);

            if (stream_read(elf_file, &elf_program_header, sizeof(ELFProgram)) != sizeof(ELFProgram))
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
