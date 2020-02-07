#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

#define GDT_ENTRY_COUNT 6

#define GDT_PRESENT 0b10010000	  // Present bit. This must be 1 for all valid selectors.
#define GDT_USER 0b01100000		  // Privilege, 2 bits. Contains the ring level, 0 = highest (kernel), 3 = lowest (user applications).
#define GDT_EXECUTABLE 0b00001000 // Executable bit. If 1 code in this segment can be executed, ie. a code selector. If 0 it is a data selector.
#define GDT_READWRITE 0b00000010  // Readable bit for code selectors //Writable bit for data selectors
#define GDT_ACCESSED 0b00000001

#define GDT_FLAGS 0b1100
#define TSS_FLAGS 0

typedef struct __packed
{
	u32 prev_tss;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iomap_base;
} TSS;

typedef struct __packed
{
	u16 size;
	u32 offset;
} GDTDescriptor;

typedef struct __packed
{
	u16 limit0_15;
	u16 base0_15;
	u8 base16_23;
	u8 acces;
	u8 limit16_19 : 4;
	u8 flags : 4;
	u8 base24_31;
} GDTEntry;

#define GDT_ENTRY(__base, __limit, __access, __flags) \
	(GDTEntry)                                        \
	{                                                 \
		.acces = (__access),                          \
		.flags = (__flags),                           \
		.base0_15 = (__base)&0xffff,                  \
		.base16_23 = ((__base) >> 16) & 0xff,         \
		.base24_31 = ((__base) >> 24) & 0xff,         \
		.limit0_15 = (__limit)&0xffff,                \
		.limit16_19 = ((__limit) >> 16) & 0x0f,       \
	}

void gdt_setup(void);

extern void gdt_flush(u32);

void set_kernel_stack(u32 stack);