#pragma once

#include <karm-base/opt.h>
#include <karm-base/std.h>

namespace x86_64 {
struct Cpuid {
    struct Result {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
    };

    enum Leaf : uint32_t {
        FEATURE_IDENTIFIER = 1,
        EXTENDED_FEATURE_IDENTIFIER = 7,
        PROC_EXTENDED_STATE_ENUMERATION = 13,
        FEATURE_IDENTIFIER_EXT = 0x80000001,
        SVM_FEATURE_IDENTIFICATION = 0x8000000A,
    };

    enum FeatureBits : uint32_t {
        // ECX
        SSSE3_SUPPORT = (1 << 9),
        SSE41_SUPPORT = (1 << 19),
        SSE42_SUPPORT = (1 << 20),
        AES_SUPPORT = (1 << 25),
        XSAVE_SUPPORT = (1 << 26),
        XSAVE_ENABLED = (1 << 27),
        AVX_SUPPORT = (1 << 28),
    };

    enum ExtendedFeatureBits : uint32_t {
        // EBX
        BIT_MANIPULATION_SUPPORT = (1 << 3),
        AVX512_SUPPORT = (1 << 16),
    };

    enum ExtendedFeatureIdentifiersBits : uint32_t {
        // ECX
        SINGLE_CORE = (1 << 1),
        SVM_SUPPORT = (1 << 2),
    };

    enum SvmFeatureIdentifiersBits  : uint32_t{
        // EDX
        SVM_NESTED_PAGING = (1 << 0),
    };

    static Opt<Cpuid::Result> cpuid(uint32_t leaf, uint32_t subleaf) {

        uint32_t cpuid_max;
        asm volatile("cpuid"
                     : "=a"(cpuid_max)
                     : "a"(leaf & 0x80000000)
                     : "rbx", "rcx", "rdx");

        if (leaf > cpuid_max) {
            return NONE;
        }

        Cpuid::Result result = {};

        asm volatile("cpuid"
                     : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
                     : "a"(leaf), "c"(subleaf));

        return result;
    }

    static bool has_xsave() 
    {
        return cpuid(Cpuid::FEATURE_IDENTIFIER, 0).unwrap().ecx & Cpuid::XSAVE_SUPPORT;   
    } 

    static bool has_avx() 
    {
        return cpuid(Cpuid::FEATURE_IDENTIFIER, 0).unwrap().ecx & Cpuid::AVX_SUPPORT;   
    } 

    static bool has_avx512() 
    {
        return cpuid(Cpuid::EXTENDED_FEATURE_IDENTIFIER, 0).unwrap().ebx & Cpuid::AVX512_SUPPORT;   
    }
    
    static bool xsave_size() 
    {
        return cpuid(Cpuid::PROC_EXTENDED_STATE_ENUMERATION, 0).unwrap().ecx;   
    }    

};
}; // namespace x86_64