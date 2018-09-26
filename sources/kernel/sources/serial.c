/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "utils.h"
#include "kernel/cpu/cpu.h"
#include "kernel/serial.h"

#define PORT 0x3f8

void serial_setup()
{
    // See: https://wiki.osdev.org/Serial_Ports
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x80);
    outb(PORT + 0, 0x03);
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x03);
    outb(PORT + 2, 0xC7);
    outb(PORT + 4, 0x0B);
}

void wait_read()
{
    while ((inb(PORT + 5) & 1) == 0)
        ;
}

void wait_write()
{
    while ((inb(PORT + 5) & 0x20) == 0)
        ;
}

char serial_getc()
{
    wait_read();
    return inb(PORT);
}

void serial_putc(char c)
{
    wait_write();
    outb(PORT, c);
}

void serial_read(char *buffer, int size)
{
    STUB(buffer, size);
}

void serial_print(const char *str)
{
    for (int i = 0; str[i]; i++)
    {
        serial_putc(str[i]);
    }
}