#pragma once

#include <libsystem/Common.h>

enum
{
    CPUID_FEAT_ECX_SSE3 = 1 << 0,
    CPUID_FEAT_ECX_PCLMUL = 1 << 1,
    CPUID_FEAT_ECX_DTES64 = 1 << 2,
    CPUID_FEAT_ECX_MONITOR = 1 << 3,
    CPUID_FEAT_ECX_DS_CPL = 1 << 4,
    CPUID_FEAT_ECX_VMX = 1 << 5,
    CPUID_FEAT_ECX_SMX = 1 << 6,
    CPUID_FEAT_ECX_EST = 1 << 7,
    CPUID_FEAT_ECX_TM2 = 1 << 8,
    CPUID_FEAT_ECX_SSSE3 = 1 << 9,
    CPUID_FEAT_ECX_CID = 1 << 10,
    CPUID_FEAT_ECX_FMA = 1 << 12,
    CPUID_FEAT_ECX_CX16 = 1 << 13,
    CPUID_FEAT_ECX_ETPRD = 1 << 14,
    CPUID_FEAT_ECX_PDCM = 1 << 15,
    CPUID_FEAT_ECX_PCIDE = 1 << 17,
    CPUID_FEAT_ECX_DCA = 1 << 18,
    CPUID_FEAT_ECX_SSE4_1 = 1 << 19,
    CPUID_FEAT_ECX_SSE4_2 = 1 << 20,
    CPUID_FEAT_ECX_x2APIC = 1 << 21,
    CPUID_FEAT_ECX_MOVBE = 1 << 22,
    CPUID_FEAT_ECX_POPCNT = 1 << 23,
    CPUID_FEAT_ECX_AES = 1 << 25,
    CPUID_FEAT_ECX_XSAVE = 1 << 26,
    CPUID_FEAT_ECX_OSXSAVE = 1 << 27,
    CPUID_FEAT_ECX_AVX = 1 << 28,

    CPUID_FEAT_EDX_FPU = 1 << 0,
    CPUID_FEAT_EDX_VME = 1 << 1,
    CPUID_FEAT_EDX_DE = 1 << 2,
    CPUID_FEAT_EDX_PSE = 1 << 3,
    CPUID_FEAT_EDX_TSC = 1 << 4,
    CPUID_FEAT_EDX_MSR = 1 << 5,
    CPUID_FEAT_EDX_PAE = 1 << 6,
    CPUID_FEAT_EDX_MCE = 1 << 7,
    CPUID_FEAT_EDX_CX8 = 1 << 8,
    CPUID_FEAT_EDX_APIC = 1 << 9,
    CPUID_FEAT_EDX_SEP = 1 << 11,
    CPUID_FEAT_EDX_MTRR = 1 << 12,
    CPUID_FEAT_EDX_PGE = 1 << 13,
    CPUID_FEAT_EDX_MCA = 1 << 14,
    CPUID_FEAT_EDX_CMOV = 1 << 15,
    CPUID_FEAT_EDX_PAT = 1 << 16,
    CPUID_FEAT_EDX_PSE36 = 1 << 17,
    CPUID_FEAT_EDX_PSN = 1 << 18,
    CPUID_FEAT_EDX_CLF = 1 << 19,
    CPUID_FEAT_EDX_DTES = 1 << 21,
    CPUID_FEAT_EDX_ACPI = 1 << 22,
    CPUID_FEAT_EDX_MMX = 1 << 23,
    CPUID_FEAT_EDX_FXSR = 1 << 24,
    CPUID_FEAT_EDX_SSE = 1 << 25,
    CPUID_FEAT_EDX_SSE2 = 1 << 26,
    CPUID_FEAT_EDX_SS = 1 << 27,
    CPUID_FEAT_EDX_HTT = 1 << 28,
    CPUID_FEAT_EDX_TM1 = 1 << 29,
    CPUID_FEAT_EDX_IA64 = 1 << 30,
    CPUID_FEAT_EDX_PBE = 1 << 31
};

struct PACKED CPUID
{
    char vendorid[16];
    union PACKED
    {
        struct PACKED
        {
            bool SSE3 : 1;
            bool PCLMUL : 1;
            bool DTES64 : 1;
            bool MONITOR : 1;
            bool DS_CPL : 1;
            bool VMX : 1;
            bool SMX : 1;
            bool EST : 1;
            bool TM2 : 1;
            bool SSSE3 : 1;
            bool CID : 1;
            bool FMA : 1;
            bool CX16 : 1;
            bool ETPRD : 1;
            bool PDCM : 1;
            bool PCIDE : 1;
            bool DCA : 1;
            bool SSE4_1 : 1;
            bool SSE4_2 : 1;
            bool x2APIC : 1;
            bool MOVBE : 1;
            bool POPCNT : 1;
            bool AES : 1;
            bool XSAVE : 1;
            bool OSXSAVE : 1;
            bool AVX : 1;
        };
        uint32_t RAW_ECX;
    };

    union PACKED
    {
        struct PACKED
        {
            bool FPU : 1;
            bool VME : 1;
            bool DE : 1;
            bool PSE : 1;
            bool TSC : 1;
            bool MSR : 1;
            bool PAE : 1;
            bool MCE : 1;
            bool CX8 : 1;
            bool APIC : 1;
            bool SEP : 1;
            bool MTRR : 1;
            bool PGE : 1;
            bool MCA : 1;
            bool CMOV : 1;
            bool PAT : 1;
            bool PSE36 : 1;
            bool PSN : 1;
            bool CLF : 1;
            bool DTES : 1;
            bool ACPI : 1;
            bool MMX : 1;
            bool FXSR : 1;
            bool SSE : 1;
            bool SSE2 : 1;
            bool SS : 1;
            bool HTT : 1;
            bool TM1 : 1;
            bool IA64 : 1;
            bool PBE : 1;
        };
        uint32_t RAW_EDX;
    };
};

CPUID cpuid();

#ifdef __cplusplus
extern "C" uint32_t cpuid_get_feature_EDX();
extern "C" uint32_t cpuid_get_feature_ECX();
#else
extern uint32_t cpuid_get_feature_EDX();
extern uint32_t cpuid_get_feature_ECX();
#endif

void cpuid_dump();
