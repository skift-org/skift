/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "kernel/cpu/idt.h"
#include "kernel/cpu/isr.h"

#include "kernel/syscalls.h"
#include "kernel/system.h"

static const char *exception_messages[32] = 
{
	"Division by zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Detected overflow",
	"Out-of-bounds",
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor segment overrun",
	"Bad TSS",
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Unknown interrupt",
	"Coprocessor fault",
	"Alignment check",
	"Machine check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void page_fault(processor_context_t* context)
{
	CPANIC(context, "PAGE FAULT at %08x", CR2());
}

extern u32 isr_vector[];
static isr_handler_t isr_handlers[32] = {0};

void isr_setup()
{
	for (u32 i = 0; i < 32; i++)
	{
		idt_entry(i, isr_vector[i], 0x08, INTGATE);
	}

	// syscall handler
	idt_entry(128, isr_vector[32], 0x08, TRAPGATE);
}

isr_handler_t isr_register(int index, isr_handler_t handler)
{
	if (index < 32)
	{
		isr_handlers[index] = handler;
		return handler;
	}

	return NULL;
}

void isr_handler(processor_context_t context)
{

	if (isr_handlers[context.int_no] != NULL)
	{
		isr_handlers[context.int_no](&context);
	}
	else
	{
		if (context.int_no == 128)
		{
			syscall_dispatcher(&context);
		}
		else
		{
			// TODO: check if the fault is in user process and terminate it.
			CPANIC(&context, "CPU EXCEPTION: '%s' (INT:%d ERR:%x) !", exception_messages[context.int_no], context.int_no, context.errcode);
		}
	}

	out8(0x20, 0x20);
}
