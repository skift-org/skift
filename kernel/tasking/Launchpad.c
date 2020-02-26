/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libfile/elf.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>

#include "kernel/tasking.h"

Result task_launch_load_elf(Task *parent_task, Task *child_task, Stream *elf_file, elf_program_t *program_header)
{
    if (program_header->vaddr <= 0x100000)
    {
        logger_error("ELF program no in user memory (0x%08x)!", program_header->vaddr);
        return ERR_EXEC_FORMAT_ERROR;
    }

    PageDirectory *parent_page_directory = task_switch_pdir(parent_task, child_task->pdir);
    paging_load_directorie(child_task->pdir);

    task_memory_map(child_task, program_header->vaddr, PAGE_ALIGN_UP(program_header->memsz) / PAGE_SIZE);
    memset((void *)program_header->vaddr, 0, program_header->memsz);

    stream_seek(elf_file, program_header->offset, WHENCE_START);
    size_t readed = stream_read(elf_file, (void *)program_header->vaddr, program_header->filesz);

    if (readed != program_header->filesz)
    {
        logger_error("Didn't readed the right ammout from the ELF file!");

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
            fshandle_acquire_lock(parent_task->handles[parent_handle_id], sheduler_running_id());
            child_task->handles[child_handle_id] = fshandle_clone(parent_task->handles[parent_handle_id]);
            fshandle_release_lock(parent_task->handles[parent_handle_id], sheduler_running_id());
        }
    }

    lock_release(parent_task->handles_lock);
}

int task_launch(Task *parent_task, Launchpad *launchpad)
{
    assert(parent_task == sheduler_running());

    int result = -ERR_FUNCTION_NOT_IMPLEMENTED;

    Stream *elf_file = stream_open(launchpad->executable, OPEN_READ);

    if (handle_has_error(elf_file))
    {
        logger_error("Failled to open ELF file %s: %s!", launchpad->executable, handle_error_string(elf_file));
        result = -handle_get_error(elf_file);

        goto cleanup_and_return;
    }

    elf_header_t elf_header;
    size_t elf_header_size = stream_read(elf_file, &elf_header, sizeof(elf_header_t));

    if (elf_header_size != sizeof(elf_header_t) || !elf_valid(&elf_header))
    {
        logger_error("Failled to load ELF file %s: bad exec format!", launchpad->executable);
        result = -ERR_EXEC_FORMAT_ERROR;

        goto cleanup_and_return;
    }

    Task *child_task = task_spawn_with_argv(parent_task, launchpad->name, (TaskEntry)elf_header.entry, (const char **)launchpad->argv, true);

    for (int i = 0; i < elf_header.phnum; i++)
    {
        elf_program_t elf_program_header;
        stream_seek(elf_file, elf_header.phoff + (elf_header.phentsize) * i, WHENCE_START);

        if (stream_read(elf_file, &elf_program_header, sizeof(elf_program_t)) != sizeof(elf_program_t))
        {
            result = -ERR_EXEC_FORMAT_ERROR;
            task_destroy(child_task);

            goto cleanup_and_return;
        }

        Result load_result = task_launch_load_elf(parent_task, child_task, elf_file, &elf_program_header);

        if (load_result != SUCCESS)
        {
            result = -ERR_EXEC_FORMAT_ERROR;
            task_destroy(child_task);

            goto cleanup_and_return;
        }
    }

    task_launch_passhandle(parent_task, child_task, launchpad);

    result = child_task->id;
    task_go(child_task);

cleanup_and_return:
    if (elf_file)
    {
        stream_close(elf_file);
    }

    return result;
}