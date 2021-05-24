#include <libutils/Prelude.h>

extern "C" void arch_x86_64_entry(void *info, uint32_t magic);

extern "C" void _kstart_stivale2(void *info)
{
    arch_x86_64_entry(info, 0x73747632);
}
