
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>

#include "archs/x86/kernel/CPUID.h"

static inline int cpuid_string(int code, int where[4])
{
    asm volatile("cpuid"
                 : "=a"(*where), "=b"(*(where + 0)),
                   "=d"(*(where + 1)), "=c"(*(where + 2))
                 : "a"(code));
    return (int)where[0];
}

CPUID cpuid()
{
    CPUID cid;
    memset(&cid.vendorid, 0, 16);
    cpuid_string(0, (int *)&cid.vendorid[0]);
    cid.RAW_ECX = cpuid_get_feature_ECX();
    cid.RAW_EDX = cpuid_get_feature_EDX();

    return cid;
}

void cpuid_dump()
{
    CPUID cid = cpuid();

    printf("\n\n\tCPUID dump:\n\t - Vendorid: %s\n\t - Features:", cid.vendorid);

    if (cid.PCLMUL)
    {
        printf(" PCLMUL");
    }

    if (cid.DTES64)
    {
        printf(" DTES64");
    }

    if (cid.MONITOR)
    {
        printf(" MONITOR");
    }

    if (cid.DS_CPL)
    {
        printf(" DS_CPL");
    }

    if (cid.VMX)
    {
        printf(" VMX");
    }

    if (cid.SMX)
    {
        printf(" SMX");
    }

    if (cid.EST)
    {
        printf(" EST");
    }

    if (cid.TM2)
    {
        printf(" TM2");
    }

    if (cid.SSSE3)
    {
        printf(" SSSE3");
    }

    if (cid.CID)
    {
        printf(" CID");
    }

    if (cid.FMA)
    {
        printf(" FMA");
    }

    if (cid.CX16)
    {
        printf(" CX16");
    }

    if (cid.ETPRD)
    {
        printf(" ETPRD");
    }

    if (cid.PDCM)
    {
        printf(" PDCM");
    }

    if (cid.PCIDE)
    {
        printf(" PCIDE");
    }

    if (cid.DCA)
    {
        printf(" DCA");
    }

    if (cid.SSE4_1)
    {
        printf(" SSE4_1");
    }

    if (cid.SSE4_2)
    {
        printf(" SSE4_2");
    }

    if (cid.x2APIC)
    {
        printf(" x2APIC");
    }

    if (cid.MOVBE)
    {
        printf(" MOVBE");
    }

    if (cid.POPCNT)
    {
        printf(" POPCNT");
    }

    if (cid.AES)
    {
        printf(" AES");
    }

    if (cid.XSAVE)
    {
        printf(" XSAVE");
    }

    if (cid.OSXSAVE)
    {
        printf(" OSXSAVE");
    }

    if (cid.AVX)
    {
        printf(" AVX");
    }

    if (cid.FPU)
    {
        printf(" FPU");
    }

    if (cid.VME)
    {
        printf(" VME");
    }

    if (cid.DE)
    {
        printf(" DE");
    }

    if (cid.PSE)
    {
        printf(" PSE");
    }

    if (cid.TSC)
    {
        printf(" TSC");
    }

    if (cid.MSR)
    {
        printf(" MSR");
    }

    if (cid.PAE)
    {
        printf(" PAE");
    }

    if (cid.MCE)
    {
        printf(" MCE");
    }

    if (cid.CX8)
    {
        printf(" CX8");
    }

    if (cid.APIC)
    {
        printf(" APIC");
    }

    if (cid.SEP)
    {
        printf(" SEP");
    }

    if (cid.MTRR)
    {
        printf(" MTRR");
    }

    if (cid.PGE)
    {
        printf(" PGE");
    }

    if (cid.MCA)
    {
        printf(" MCA");
    }

    if (cid.CMOV)
    {
        printf(" CMOV");
    }

    if (cid.PAT)
    {
        printf(" PAT");
    }

    if (cid.PSE36)
    {
        printf(" PSE36");
    }

    if (cid.PSN)
    {
        printf(" PSN");
    }

    if (cid.CLF)
    {
        printf(" CLF");
    }

    if (cid.DTES)
    {
        printf(" DTES");
    }

    if (cid.ACPI)
    {
        printf(" ACPI");
    }

    if (cid.MMX)
    {
        printf(" MMX");
    }

    if (cid.FXSR)
    {
        printf(" FXSR");
    }

    if (cid.SSE)
    {
        printf(" SSE");
    }

    if (cid.SSE2)
    {
        printf(" SSE2");
    }

    if (cid.SSE3)
    {
        printf(" SSE3");
    }

    if (cid.SS)
    {
        printf(" SS");
    }

    if (cid.HTT)
    {
        printf(" HTT");
    }

    if (cid.TM1)
    {
        printf(" TM1");
    }

    if (cid.IA64)
    {
        printf(" IA64");
    }

    if (cid.PBE)
    {
        printf(" PBE");
    }

    printf("\n");
}
