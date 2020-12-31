#include <libsystem/Common.h>

extern "C" void arch_main(void *info, uint32_t magic);

extern "C" void _kstart_stivale2(void *info)
{
    arch_main(info, 0x73747632);
}
