#include <libsystem/core/Plugs.h>

#include "archs/x86/COM.h"
#include "archs/x86/FPU.h"
#include "archs/x86/PIC.h"
#include "archs/x86/PIT.h"
#include "archs/x86_64/GDT.h"
#include "archs/x86_64/IDT.h"
#include "system/graphics/Graphics.h"

namespace Arch::x86_64
{

extern "C" void arch_x86_64_entry(void *info, uint32_t magic)
{
    com_initialize(COM1);
    com_initialize(COM2);
    com_initialize(COM3);
    com_initialize(COM4);

    auto handover = handover_initialize(info, magic);

    graphic_early_initialize(handover);

    if (handover->memory_usable < 127 * 1024)
    {
        system_panic("No enoughs memory (%uKio)!", handover->memory_usable / 1024);
    }

    gdt_initialize();
    idt_initialize();
    pic_initialize();
    fpu_initialize();
    pit_initialize(1000);

    system_main(handover);

    ASSERT_NOT_REACHED();
}

} // namespace Arch::x86_64
