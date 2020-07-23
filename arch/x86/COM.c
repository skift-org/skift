#include <libsystem/thread/Atomic.h>

#include "arch/x86/COM.h"
#include "arch/x86/x86.h"

void com_wait_write(COMPort port)
{
    while ((in8(port + 5) & 0x20) == 0)
    { /* do nothing */
    }
}

void com_wait_read(COMPort port)
{
    while ((in8(port + 5) & 0x01) == 0)
    { /* do nothing */
    }
}

void com_putc(COMPort port, char c)
{
    com_wait_write(port);
    out8(port, c);
}

char com_getc(COMPort port)
{
    com_wait_read(port);
    return in8(port);
}

size_t com_write(COMPort port, const void *buffer, size_t size)
{
    atomic_begin();

    for (size_t i = 0; i < size; i++)
    {
        com_putc(port, ((const char *)buffer)[i]);
    }

    atomic_end();

    return size;
}

void com_initialize(COMPort port)
{
    out8(port + 2, 0);
    out8(port + 3, 0x80);
    out8(port + 0, 115200 / 9600);
    out8(port + 1, 0);
    out8(port + 3, 0x03);
    out8(port + 4, 0);
    out8(port + 1, 0x01);
}
