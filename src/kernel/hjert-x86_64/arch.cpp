#include <hjert-core/arch.h>
#include <karm-debug/logger.h>

#include <hal-x86_64/com.h>
#include <hal-x86_64/cpuid.h>
#include <hal-x86_64/gdt.h>
#include <hal-x86_64/idt.h>
#include <hal-x86_64/pic.h>
#include <hal-x86_64/pit.h>

#include "ints.h"

namespace Hjert::Arch {

static x86_64::Com _com1 = x86_64::Com::com1();

static x86_64::DualPic _pic = x86_64::DualPic::dualPic();
static x86_64::Pit _pit = x86_64::Pit::pit();

static x86_64::Gdt _gdt{};
static x86_64::GdtDesc _gdtDesc{_gdt};

static x86_64::Idt _idt{};
static x86_64::IdtDesc _idtDesc{_idt};

Error init(Handover::Payload &) {
    _com1.init();

    _gdtDesc.load();

    for (size_t i = 0; i < x86_64::Idt::LEN; i++) {
        _idt.entries[i] = x86_64::IdtEntry{_intVec[i], 0, x86_64::IdtEntry::GATE};
    }

    _idtDesc.load();

    _pic.init();
    _pit.init(1000);

    x86_64::sti();

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

static uint64_t _tick = 0;

static char const *_faultMsg[32] = {
    "division-by-zero",
    "debug",
    "non-maskable-interrupt",
    "breakpoint",
    "detected-overflow",
    "out-of-bounds",
    "invalid-opcode",
    "no-coprocessor",
    "double-fault",
    "coprocessor-segment-overrun",
    "bad-tss",
    "segment-not-present",
    "stack-fault",
    "general-protection-fault",
    "page-fault",
    "unknown-interrupt",
    "coprocessor-fault",
    "alignment-check",
    "machine-check",
    "simd-floating-point-exception",
    "virtualization-exception",
    "control-protection-exception",
    "reserved",
    "hypervisor-injection-exception",
    "vmm-communication-exception",
    "security-exception",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
};

extern "C" uintptr_t _intDispatch(uintptr_t rsp) {
    auto frame = reinterpret_cast<Frame *>(rsp);

    if (frame->intNo < 32) {
        Debug::lfatal("CPU Exception: {} (err={}, ip={x}, sp={x}, cr2={x}, cr3={x})", _faultMsg[frame->intNo], frame->errNo, frame->rip, frame->rsp, x86_64::rdcr2(), x86_64::rdcr3());
    } else {
        int irq = frame->intNo - 32;

        if (irq == 0) {
            _tick++;
            if (_tick % 1000 == 0) {
                Debug::linfo("tick: {}", _tick);
            }
        } else {
            Debug::linfo("irq: {}", irq);
        }
    }

    _pic.ack(frame->intNo);

    return rsp;
}

} // namespace Hjert::Arch
