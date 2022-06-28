
#include "gdt.h"
namespace Hjert::Arch::x86_64 {
static Gdt _gdt = Gdt{};

static GdtDesc _gdt_desc = GdtDesc{
    sizeof(Gdt) - 1,
    &_gdt,
};

extern "C" void gdtUpdate(void *ptr);

void gdtInitialize() {
    gdtUpdate(&_gdt_desc);
}
} // namespace Hjert::Arch::x86_64