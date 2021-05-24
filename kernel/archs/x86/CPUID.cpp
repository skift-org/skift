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
