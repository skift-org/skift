#include <stdio.h>

#include "kernel/dumping.h"
#include "kernel/version.h"

void dump_multiboot_info(multiboot_info_t * info)
{
    printf("&fMultiboot info:&7");
    printf("\n\tbootloader name: %s", info->boot_loader_name);
    printf("\n\tkernel command line: %s", info->cmdline);
    printf("\n\tmemory: LOWER=%dko, UPPER=%dko", info->mem_lower, info->mem_upper);
    printf("\n\tframebuffer: addr=%d, size=%dx%d", info->framebuffer_addr, info->framebuffer_width, info->framebuffer_height);
    puts("\n");
}

void dump_kernel_info()
{
    printf("&fKernel info:&7");
    printf("\n\tkernel uname: "); printf(KERNEL_UNAME);
    puts("\n");
}