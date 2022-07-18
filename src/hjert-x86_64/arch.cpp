#include <hjert/arch.h>
#include <karm-debug/logger.h>

#include <arch-x86_64/com.h>
#include <arch-x86_64/cpuid.h>
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

    auto branding = x86_64::Cpuid::branding();

    Debug::linfo("Cpu vendor: {}", branding.vendor());
    Debug::linfo("Cpu brand: {}", branding.brand());

    return OK;
}

Io::TextWriter<Embed::Encoding> &loggerOut() {
    return _com1;
}

void stopCpu() {
    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

} // namespace Hjert::Arch
