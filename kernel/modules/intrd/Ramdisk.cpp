
#include <abi/Result.h>
#include <libfile/TARArchive.h>

#include "archs/Arch.h"
#include "system/Streams.h"
#include "system/memory/Memory.h"
#include "system/modules/Modules.h"
#include "system/scheduling/Scheduler.h"

void ramdisk_load(Module *module)
{
    TARBlock block;
    for (size_t i = 0; tar_read((void *)module->range.base(), &block, i); i++)
    {
        auto file_path = IO::Path::parse(block.name);

        if (block.name[strlen(block.name) - 1] == '/')
        {
            HjResult result = scheduler_running()->domain().mkdir(file_path);

            if (result != SUCCESS)
            {
                Kernel::logln("Failed to create directory {}: {}", block.name, result_to_string(result));
            }
        }
        else if ((block.typeflag & 8) == 0 || (block.typeflag & 8) == 5)
        {
            auto result_or_handle = scheduler_running()->domain().open(file_path, HJ_OPEN_WRITE | HJ_OPEN_CREATE);

            if (!result_or_handle.success())
            {
                Kernel::logln("Failed to open file {}! {}", block.name, result_to_string(result_or_handle.result()));
                continue;
            }

            auto handle = result_or_handle.unwrap();

            auto result_or_written = handle->write(block.data, block.size);

            if (!result_or_written.success())
            {
                Kernel::logln("Failed to write file: {}", result_to_string(result_or_written.result()));
            }
        }
    }

    memory_free(Arch::kernel_address_space(), module->range);

    Kernel::logln("Loading ramdisk succeeded.");
}
