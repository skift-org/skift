#pragma once

#include "asm.h"
#include "cpuid.h"

namespace x86_64 {

inline void simdInit() {
    wrcr0(rdcr0() & ~((u64)CR0_EMULATION));
    wrcr0(rdcr0() | CR0_MONITOR_CO_PROCESSOR);
    wrcr0(rdcr0() | CR0_NUMERIC_ERROR_ENABLE);

    wrcr4(rdcr4() | CR4_FXSR_ENABLE);
    wrcr4(rdcr4() | CR4_SIMD_EXCEPTION_SUPPORT);

    if (Cpuid::hasXsave()) {
        wrcr4(rdcr4() | CR4_XSAVE_ENABLE);

        u64 xcr0 = 0;

        xcr0 |= XCR0_XSAVE_SAVE_X87;
        xcr0 |= XCR0_XSAVE_SAVE_SSE;

        if (Cpuid::hasAvx()) {
            xcr0 |= XCR0_AVX_ENABLE;
        }

        if (Cpuid::hasAvx512()) {
            xcr0 |= XCR0_AVX512_ENABLE;
            xcr0 |= XCR0_ZMM0_15_ENABLE;
            xcr0 |= XCR0_ZMM16_32_ENABLE;
        }

        wrxcr(0, xcr0);
    }

    fninit();
}

inline usize simdContextSize() {
    if (Cpuid::hasXsave()) {
        return Cpuid::xsaveSize();
    }

    return 512;
}

inline void simdSaveContext(void* ptr) {
    if (Cpuid::hasXsave()) {
        xsave(ptr);
    } else {
        fxsave(ptr);
    }
}

inline void simdLoadContext(void* ptr) {
    if (Cpuid::hasXsave()) {
        xrstor(ptr);
    } else {
        fxrstor(ptr);
    }
}

inline void simdInitContext(void* ptr) {
    fninit();
    simdSaveContext(ptr);
}

} // namespace x86_64
