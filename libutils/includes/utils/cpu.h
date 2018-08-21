#pragma once

#define cli() asm volatile("cli")
#define sti() asm volatile("sti")
#define hlt() asm volatile("hlt")