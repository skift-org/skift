#include <thirdparty/limine/stivale/stivale2.h>

extern int __stack_top;

extern "C" void arch_main(void *info, uint32_t magic);
extern "C" void _kstart_stivale2(void *info);

struct stivale2_header_tag_smp smp_request = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next = 0,
    },
    .flags = 0,
};

struct stivale2_header_tag_framebuffer framebuffer_request = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&smp_request,
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 32,
};

__attribute__((section(".stivale2hdr"), used)) struct stivale2_header header2 = {
    .entry_point = (uint64_t)_kstart_stivale2,
    .stack = (uint64_t)&__stack_top,
    .flags = 0,
    .tags = (uint64_t)&framebuffer_request,
};

void _kstart_stivale2(void *info)
{
    arch_main(info, 0x73747632);
}
