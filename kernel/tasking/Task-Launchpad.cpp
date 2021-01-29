#include <libfile/ELF.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>

#include "kernel/interrupts/Interupts.h"
#include "kernel/scheduling/Scheduler.h"
#include "kernel/tasking/Task-Lanchpad.h"
#include "kernel/tasking/Task-Memory.h"
#include "kernel/tasking/Task.h"

template <typename TELFFormat>
struct ELFLoader
{
    using Header = TELFFormat::Header;
    using Section = TELFFormat::Section;
    using Program = TELFFormat::Program;
    using Symbole = TELFFormat::Symbole;

    static Result load_program(Task *task, Stream *elf_file, Program *program_header)
    {
        if (program_header->vaddr <= 0x100000)
        {
            logger_error("ELF program no in user memory (0x%08x)!", program_header->vaddr);
            return ERR_EXEC_FORMAT_ERROR;
        }

        void *parent_address_space = task_switch_address_space(scheduler_running(), task->address_space);

        MemoryRange range = MemoryRange::around_non_aligned_address(program_header->vaddr, program_header->memsz);

        task_memory_map(task, range.base(), range.size(), MEMORY_CLEAR);

        stream_seek(elf_file, program_header->offset, WHENCE_START);
        size_t read = stream_read(elf_file, (void *)program_header->vaddr, program_header->filesz);

        if (read != program_header->filesz)
        {
            logger_error("Didn't read the right amount from the ELF file!");

            task_switch_address_space(scheduler_running(), parent_address_space);

            return ERR_EXEC_FORMAT_ERROR;
        }
        else
        {
            task_switch_address_space(scheduler_running(), parent_address_space);

            return SUCCESS;
        }
    }

    static Result load(Task *task, Stream *elf_file)
    {
        Header elf_header;
        size_t elf_header_size = stream_read(elf_file, &elf_header, sizeof(Header));

        if (elf_header_size != sizeof(Header) || !elf_header.valid())
        {
            return ERR_EXEC_FORMAT_ERROR;
        }

        task_set_entry(task, reinterpret_cast<TaskEntryPoint>(elf_header.entry), true);

        for (int i = 0; i < elf_header.phnum; i++)
        {
            Program elf_program_header;
            stream_seek(elf_file, elf_header.phoff + elf_header.phentsize * i, WHENCE_START);

            if (stream_read(elf_file, &elf_program_header, sizeof(Program)) != sizeof(Program))
            {
                return ERR_EXEC_FORMAT_ERROR;
            }

            Result result = load_program(task, elf_file, &elf_program_header);

            if (result != SUCCESS)
            {
                return result;
            }
        }

        return SUCCESS;
    }
};

void task_pass_argc_argv_env(Task *task, Launchpad *launchpad)
{
    void *parent_address_space = task_switch_address_space(scheduler_running(), task->address_space);

    uintptr_t argv_list[PROCESS_ARG_COUNT] = {};

    for (int i = 0; i < launchpad->argc; i++)
    {
        task_user_stack_push(task, "\0", 1); // null terminate the arg string
        argv_list[i] = task_user_stack_push(task, launchpad->argv[i].buffer, launchpad->argv[i].size);
    }

    uintptr_t argv_list_ref = task_user_stack_push(task, &argv_list, sizeof(argv_list));

    task_user_stack_push(task, "\0", 1); // null terminate the env string
    uintptr_t env_ref = task_user_stack_push(task, launchpad->env, launchpad->env_size);

    task_user_stack_push_ptr(task, (void *)env_ref);
    task_user_stack_push_ptr(task, (void *)argv_list_ref);
    task_user_stack_push_long(task, launchpad->argc);

    task_switch_address_space(scheduler_running(), parent_address_space);
}

void task_pass_handles(Task *parent_task, Task *child_task, Launchpad *launchpad)
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
            parent_task->handles[parent_handle_id]->acquire(scheduler_running_id());
            child_task->handles[child_handle_id] = new FsHandle(*parent_task->handles[parent_handle_id]);
            parent_task->handles[parent_handle_id]->release(scheduler_running_id());
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

    interrupts_retain();
    Task *task = task_create(parent_task, launchpad->name, true);
    interrupts_release();

#ifdef __x86_64__
    Result result = ELFLoader<ELF64>::load(task, elf_file);
#else
    Result result = ELFLoader<ELF32>::load(task, elf_file);
#endif

    if (result != SUCCESS)
    {
        task_destroy(task);
        return result;
    }

    task_pass_argc_argv_env(task, launchpad);

    task_pass_handles(parent_task, task, launchpad);

    *pid = task->id;

    task_go(task);

    return SUCCESS;
}
