#include <libsystem/io/Stream.h>
#include <string.h>

#include "archs/x86/CPUID.h"

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

    stream_format(out_stream, "\n\n\tCPUID dump:\n\t - Vendorid: %s\n\t - Features:", cid.vendorid);

    if (cid.PCLMUL)
    {
        stream_format(out_stream, " PCLMUL");
    }

    if (cid.DTES64)
    {
        stream_format(out_stream, " DTES64");
    }

    if (cid.MONITOR)
    {
        stream_format(out_stream, " MONITOR");
    }

    if (cid.DS_CPL)
    {
        stream_format(out_stream, " DS_CPL");
    }

    if (cid.VMX)
    {
        stream_format(out_stream, " VMX");
    }

    if (cid.SMX)
    {
        stream_format(out_stream, " SMX");
    }

    if (cid.EST)
    {
        stream_format(out_stream, " EST");
    }

    if (cid.TM2)
    {
        stream_format(out_stream, " TM2");
    }

    if (cid.SSSE3)
    {
        stream_format(out_stream, " SSSE3");
    }

    if (cid.CID)
    {
        stream_format(out_stream, " CID");
    }

    if (cid.FMA)
    {
        stream_format(out_stream, " FMA");
    }

    if (cid.CX16)
    {
        stream_format(out_stream, " CX16");
    }

    if (cid.ETPRD)
    {
        stream_format(out_stream, " ETPRD");
    }

    if (cid.PDCM)
    {
        stream_format(out_stream, " PDCM");
    }

    if (cid.PCIDE)
    {
        stream_format(out_stream, " PCIDE");
    }

    if (cid.DCA)
    {
        stream_format(out_stream, " DCA");
    }

    if (cid.SSE4_1)
    {
        stream_format(out_stream, " SSE4_1");
    }

    if (cid.SSE4_2)
    {
        stream_format(out_stream, " SSE4_2");
    }

    if (cid.x2APIC)
    {
        stream_format(out_stream, " x2APIC");
    }

    if (cid.MOVBE)
    {
        stream_format(out_stream, " MOVBE");
    }

    if (cid.POPCNT)
    {
        stream_format(out_stream, " POPCNT");
    }

    if (cid.AES)
    {
        stream_format(out_stream, " AES");
    }

    if (cid.XSAVE)
    {
        stream_format(out_stream, " XSAVE");
    }

    if (cid.OSXSAVE)
    {
        stream_format(out_stream, " OSXSAVE");
    }

    if (cid.AVX)
    {
        stream_format(out_stream, " AVX");
    }

    if (cid.FPU)
    {
        stream_format(out_stream, " FPU");
    }

    if (cid.VME)
    {
        stream_format(out_stream, " VME");
    }

    if (cid.DE)
    {
        stream_format(out_stream, " DE");
    }

    if (cid.PSE)
    {
        stream_format(out_stream, " PSE");
    }

    if (cid.TSC)
    {
        stream_format(out_stream, " TSC");
    }

    if (cid.MSR)
    {
        stream_format(out_stream, " MSR");
    }

    if (cid.PAE)
    {
        stream_format(out_stream, " PAE");
    }

    if (cid.MCE)
    {
        stream_format(out_stream, " MCE");
    }

    if (cid.CX8)
    {
        stream_format(out_stream, " CX8");
    }

    if (cid.APIC)
    {
        stream_format(out_stream, " APIC");
    }

    if (cid.SEP)
    {
        stream_format(out_stream, " SEP");
    }

    if (cid.MTRR)
    {
        stream_format(out_stream, " MTRR");
    }

    if (cid.PGE)
    {
        stream_format(out_stream, " PGE");
    }

    if (cid.MCA)
    {
        stream_format(out_stream, " MCA");
    }

    if (cid.CMOV)
    {
        stream_format(out_stream, " CMOV");
    }

    if (cid.PAT)
    {
        stream_format(out_stream, " PAT");
    }

    if (cid.PSE36)
    {
        stream_format(out_stream, " PSE36");
    }

    if (cid.PSN)
    {
        stream_format(out_stream, " PSN");
    }

    if (cid.CLF)
    {
        stream_format(out_stream, " CLF");
    }

    if (cid.DTES)
    {
        stream_format(out_stream, " DTES");
    }

    if (cid.ACPI)
    {
        stream_format(out_stream, " ACPI");
    }

    if (cid.MMX)
    {
        stream_format(out_stream, " MMX");
    }

    if (cid.FXSR)
    {
        stream_format(out_stream, " FXSR");
    }

    if (cid.SSE)
    {
        stream_format(out_stream, " SSE");
    }

    if (cid.SSE2)
    {
        stream_format(out_stream, " SSE2");
    }

    if (cid.SSE3)
    {
        stream_format(out_stream, " SSE3");
    }

    if (cid.SS)
    {
        stream_format(out_stream, " SS");
    }

    if (cid.HTT)
    {
        stream_format(out_stream, " HTT");
    }

    if (cid.TM1)
    {
        stream_format(out_stream, " TM1");
    }

    if (cid.IA64)
    {
        stream_format(out_stream, " IA64");
    }

    if (cid.PBE)
    {
        stream_format(out_stream, " PBE");
    }

    stream_format(out_stream, "\n");
}
