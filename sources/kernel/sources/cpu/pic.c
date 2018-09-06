// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include "kernel/cpu/idt.h"

#define PIC_WAIT() \
    do { \
      asm volatile("jmp 1f\n\t" \
                  "1:\n\t" \
                  "    jmp 2f\n\t" \
                  "2:"); \
    } while (0)


#define PIC1           0x20
#define PIC1_COMMAND   PIC1
#define PIC1_OFFSET    0x20
#define PIC1_DATA      (PIC1+1)

#define PIC2           0xA0
#define PIC2_COMMAND   PIC2
#define PIC2_OFFSET    0x28
#define PIC2_DATA      (PIC2+1)

#define ICW1_ICW4      0x01
#define ICW1_INIT      0x10


void pic_setup()
{
	/* Cascade initialization */
	outb(PIC1_COMMAND, ICW1_INIT|ICW1_ICW4); PIC_WAIT();
	outb(PIC2_COMMAND, ICW1_INIT|ICW1_ICW4); PIC_WAIT();

	/* Remap */
	outb(PIC1_DATA, PIC1_OFFSET); PIC_WAIT();
	outb(PIC2_DATA, PIC2_OFFSET); PIC_WAIT();

	/* Cascade identity with slave PIC at IRQ2 */
	outb(PIC1_DATA, 0x04); PIC_WAIT();
	outb(PIC2_DATA, 0x02); PIC_WAIT();

	/* Request 8086 mode on each PIC */
	outb(PIC1_DATA, 0x01); PIC_WAIT();
	outb(PIC2_DATA, 0x01); PIC_WAIT();
}