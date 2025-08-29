#pragma once

import Karm.Core;

namespace x86_64 {

union Cpuid {
    struct {
        u32 eax;
        u32 ebx;
        u32 ecx;
        u32 edx;
    };

    Array<u32, 4> _els;
    Array<char, 16> _str;

    static inline Cpuid cpuid(u32 leaf = 0, u32 subleaf = 0) {
        u32 maxLeaf = 0;

        asm volatile("cpuid"
                     : "=a"(maxLeaf)
                     : "a"(leaf & 0x80000000)
                     : "rbx", "rcx", "rdx");

        if (leaf > maxLeaf) [[unlikely]]
            panic("cpuid leaf out of range");

        Cpuid result{};

        asm volatile("cpuid"
                     : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
                     : "a"(leaf), "c"(subleaf));

        return result;
    }

    struct Branding {
        Array<char, 12> _vendor{};
        Array<char, 48> _brand{};

        Str vendor() const {
            return _vendor;
        }

        Str brand() const {
            return _brand;
        }
    };

    static Array<char, 12> _vendor() {
        union {
            Array<u32, 3> regs;
            Array<char, 12> str;
        } buf{};

        Cpuid result = cpuid(0);
        buf.regs[0] = result.ebx;
        buf.regs[1] = result.edx;
        buf.regs[2] = result.ecx;

        return {buf.str};
    }

    static Array<char, 48> _brand() {
        union [[gnu::packed]] {
            Array<Cpuid, 4> ids;
            Array<char, 48> str;
        } buf{};

        buf.ids[0] = cpuid(0x80000002);
        buf.ids[1] = cpuid(0x80000003);
        buf.ids[2] = cpuid(0x80000004);
        buf.ids[3] = cpuid(0x80000005);
        return buf.str;
    }

    static Branding branding() {
        return {_vendor(), _brand()};
    }

    static bool hasXsave() {
        return cpuid(0x01, 0x00).ecx & (1 << 26);
    }

    static bool hasAvx() {
        return cpuid(0x01, 0x00).ecx & (1 << 28);
    }

    static bool hasAvx512() {
        return cpuid(0x7, 0).ebx & (1 << 16);
    }

    static bool xsaveSize() {
        return cpuid(0x0d, 0).ecx;
    }
};

} // namespace x86_64
