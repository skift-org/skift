/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include <string.h>

#include "kernel/cpu/cpuid.h"

static inline int cpuid_string(int code, int where[4]) 
{
    asm volatile ("cpuid":"=a"(*where),"=b"(*(where+0)),
                "=d"(*(where+1)),"=c"(*(where+2)):"a"(code));
    return (int)where[0];
}
 
string cpuid_get_vendorid() 
{
    static char s[16] = "BogusProces!";
    cpuid_string(0, (int*)(s));
    return s;
}

cpuid_t cpuid()
{
    cpuid_t cid;
    memset(&cid.vendorid, 0, 16);
    cpuid_string(0, (int*)&cid.vendorid[0]);
    cid.RAW_ECX = cpuid_get_feature_ECX();
    cid.RAW_EDX = cpuid_get_feature_EDX();
    
    return cid;
}

void cpuid_dump()
{
    cpuid_t cid = cpuid();
    
    printf("&fCPUID dump:&7\n\tVendorid: %s\n\tFeatures:", cid.vendorid);    

    // if (cid.PCLMUL ) { puts(" PCLMUL"); }      
    // if (cid.DTES64 ) { puts(" DTES64"); }       
    // if (cid.MONITOR) { puts(" MONITOR"); }    
    // if (cid.DS_CPL ) { puts(" DS_CPL"); }    
    // if (cid.VMX    ) { puts(" VMX"); }    
    // if (cid.SMX    ) { puts(" SMX"); }    
    // if (cid.EST    ) { puts(" EST"); }    
    // if (cid.TM2    ) { puts(" TM2"); }    
    // if (cid.SSSE3  ) { puts(" SSSE3"); }    
    // if (cid.CID    ) { puts(" CID"); }    
    // if (cid.FMA    ) { puts(" FMA"); } 
    // if (cid.CX16   ) { puts(" CX16"); }  
    // if (cid.ETPRD  ) { puts(" ETPRD"); }  
    // if (cid.PDCM   ) { puts(" PDCM"); }  
    // if (cid.PCIDE  ) { puts(" PCIDE"); }  
    // if (cid.DCA    ) { puts(" DCA"); }  
    // if (cid.SSE4_1 ) { puts(" SSE4_1"); }  
    // if (cid.SSE4_2 ) { puts(" SSE4_2"); }  
    // if (cid.x2APIC ) { puts(" x2APIC"); }  
    // if (cid.MOVBE  ) { puts(" MOVBE"); }  
    // if (cid.POPCNT ) { puts(" POPCNT"); }  
    // if (cid.AES    ) { puts(" AES"); }  
    // if (cid.XSAVE  ) { puts(" XSAVE"); }  
    // if (cid.OSXSAVE) { puts(" OSXSAVE"); }  
    // if (cid.AVX    ) { puts(" AVX"); }  
    if (cid.FPU    ) { puts(" FPU"); }   
    // if (cid.VME    ) { puts(" VME"); }   
    // if (cid.DE     ) { puts(" DE"); }   
    // if (cid.PSE    ) { puts(" PSE"); }   
    // if (cid.TSC    ) { puts(" TSC"); }   
    // if (cid.MSR    ) { puts(" MSR"); }   
    if (cid.PAE    ) { puts(" PAE"); }   
    // if (cid.MCE    ) { puts(" MCE"); }   
    // if (cid.CX8    ) { puts(" CX8"); }   
    // if (cid.APIC   ) { puts(" APIC"); }   
    // if (cid.SEP    ) { puts(" SEP"); }   
    // if (cid.MTRR   ) { puts(" MTRR"); }   
    // if (cid.PGE    ) { puts(" PGE"); }   
    // if (cid.MCA    ) { puts(" MCA"); }   
    // if (cid.CMOV   ) { puts(" CMOV"); }   
    // if (cid.PAT    ) { puts(" PAT"); }   
    // if (cid.PSE36  ) { puts(" PSE36"); }   
    // if (cid.PSN    ) { puts(" PSN"); }   
    // if (cid.CLF    ) { puts(" CLF"); }   
    // if (cid.DTES   ) { puts(" DTES"); }   
    // if (cid.ACPI   ) { puts(" ACPI"); }   
    // if (cid.MMX    ) { puts(" MMX"); }   
    // if (cid.FXSR   ) { puts(" FXSR"); }   
    if (cid.SSE    ) { puts(" SSE"); }   
    if (cid.SSE2   ) { puts(" SSE2"); }   
    if (cid.SSE3   ) { puts(" SSE3"); } 
    // if (cid.SS     ) { puts(" SS"); }   
    // if (cid.HTT    ) { puts(" HTT"); }   
    // if (cid.TM1    ) { puts(" TM1"); }   
    // if (cid.IA64   ) { puts(" IA64"); }   
    // if (cid.PBE    ) { puts(" PBE"); }
    puts("&7\n");
}