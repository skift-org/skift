
#include "archs/x86/PIC.h"
#include "archs/x86/IOPort.h"

#define PIC_WAIT()                    \
    do                                \
    {                                 \
        asm volatile("jmp 1f\n\t"     \
                     "1:\n\t"         \
                     "    jmp 2f\n\t" \
                     "2:");           \
    } while (0)

#define PIC1 0x20
#define PIC1_COMMAND PIC1
#define PIC1_OFFSET 0x20
#define PIC1_DATA (PIC1 + 1)

#define PIC2 0xA0
#define PIC2_COMMAND PIC2
#define PIC2_OFFSET 0x28
#define PIC2_DATA (PIC2 + 1)

#define ICW1_ICW4 0x01
#define ICW1_INIT 0x10

void pic_initialize()
{
    /* Cascade initialization */
    out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    PIC_WAIT();
    out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    PIC_WAIT();

    /* Remap */
    out8(PIC1_DATA, PIC1_OFFSET);
    PIC_WAIT();
    out8(PIC2_DATA, PIC2_OFFSET);
    PIC_WAIT();

    /* Cascade identity with slave PIC at IRQ2 */
    out8(PIC1_DATA, 0x04);
    PIC_WAIT();
    out8(PIC2_DATA, 0x02);
    PIC_WAIT();

    /* Request 8086 mode on each PIC */
    out8(PIC1_DATA, 0x01);
    PIC_WAIT();
    out8(PIC2_DATA, 0x01);
    PIC_WAIT();

    out8(PIC1_DATA, 0x00);
    PIC_WAIT();
    out8(PIC2_DATA, 0x00);
    PIC_WAIT();
}

void pic_ack(int intno)
{
    if (intno >= 40)
    {
        out8(0xA0, 0x20);
    }

    out8(0x20, 0x20);
}

void pic_disable()
{
    out8(PIC2_DATA, 0xff);
    out8(PIC1_DATA, 0xff);
}
