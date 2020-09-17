#include "kernel/handover/Stivale1.h"

__attribute__((section(".stivalehdr"), used)) StivaleHeader header = {
    .stack = 0,
    .flags = 1,
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 32,
    .entry_point = 0,
};
