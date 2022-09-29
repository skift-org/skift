#include <hjert/arch.h>
#include <karm-debug/logger.h>

#include <hal-x86_64/com.h>
#include <hal-x86_64/cpuid.h>
#include <hal-x86_64/gdt.h>
#include <hal-x86_64/idt.h>

#include "ints.h"

namespace Hjert::Arch {

static x86_64::Com _com1{x86_64::Com::COM1};

static x86_64::Gdt _gdt{};
static x86_64::GdtDesc _gdtDesc{_gdt};

static x86_64::Idt _idt{};
static x86_64::IdtDesc _idtDesc{_idt};

Error init() {
    _com1.init();
    _gdtDesc.load();

    for (size_t i = 0; i < x86_64::Idt::LEN; i++) {
        _idt.entries[i] = x86_64::IdtEntry{_intVec[i], 0, x86_64::IdtEntry::GATE};
    }

    _idtDesc.load();

    auto branding = x86_64::Cpuid::branding();

    Debug::linfo("CPU vendor: {}", branding.vendor());
    Debug::linfo("CPU brand: {}", branding.brand());

    return OK;
}

Io::TextWriter<> &loggerOut() {
    return _com1;
}

void stopAll() {
    while (true) {
        x86_64::cli();
        x86_64::hlt();
    }
}

void idleCpu() {
    while (true)
        x86_64::hlt();
}

} // namespace Hjert::Arch
