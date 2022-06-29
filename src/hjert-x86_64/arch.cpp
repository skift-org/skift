#include <hjert/arch.h>

#include <arch-x86_64/com.h>
#include <arch-x86_64/gdt.h>

namespace Hjert::Arch {

static x86_64::Com _com1{x86_64::Com::COM1};

static x86_64::Gdt _gdt{};
static x86_64::GdtDesc _gdtDesc{_gdt};

Error init() {
    _com1.init();
    _gdtDesc.load();

    return OK;
}

Result<size_t> writeLog(Str str) {
    return _com1.writeStr(str);
}

void stopCpu() {
    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

} // namespace Hjert::Arch
