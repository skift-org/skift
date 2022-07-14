#include <hjert/arch.h>

#include <arch-x86_64/com.h>
#include <arch-x86_64/gdt.h>
#include <arch-x86_64/idt.h>

namespace Hjert::Arch {

static x86_64::Com _com1{x86_64::Com::COM1};

static constexpr x86_64::Gdt _gdt{};
static x86_64::GdtDesc _gdtDesc{_gdt};

static constexpr x86_64::Idt _idt{};
static x86_64::IdtDesc _idtDesc{_idt};

Error init() {
    _com1.init();
    _gdtDesc.load();
    _idtDesc.load();

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
