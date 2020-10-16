#include <libsystem/Common.h>

extern "C" void arch_main(void *info, uint32_t magic);

extern "C" void _kstart_stivale1(void *info)
{
    arch_main(info, 0x7374766c);
}
