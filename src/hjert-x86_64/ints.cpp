#include <hjert/arch.h>
#include <karm-debug/logger.h>

#include <arch-x86_64/asm.h>

#include "ints.h"

namespace Hjert::Arch {

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
        Debug::lpanic("{}: err:{} ip:{x} sp:{x} cr2:{x} cr3:{x}", _faultMsg[frame->intNo], frame->errNo, frame->rip, frame->rsp, x86_64::rdcr2(), x86_64::rdcr3());
    }

    return rsp;
}

} // namespace Hjert::Arch
