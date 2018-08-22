// This file is part of "skiftOS" licensed under the MIT License.
// See: LICENSE.md
// Project URL: github.com/maker-dev/skift

#include "devices/uart.h"
#include "cpu/cpu.h"

void uart_setup()
{
    uart_open(COM1);
    uart_open(COM2);
    uart_open(COM3);
    uart_open(COM4);
}

void uart_open(u16 port)
{
    outb(port + 1, 0x00); // Disable all interrupts
    outb(port + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00); //                  (hi byte)
    outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

int uart_is_transmit_empty(uart_port_t port)
{
    return inb(port + 5) & 0x20;
}

void uart_put(uart_port_t port, char a)
{
    while (uart_is_transmit_empty(port) == 0)
        ;
    outb(port, a);
}

const char *const color_code[] =
{
    "0;30", // back
    "0;34", // blue
    "0;32", // green
    "0;36", // cyan
    "0;31", // red
    "0;35", // purple
    "0;33", // brown
    "0;37", // light gray

    "1;30", // dark gray
    "1;34", // light blue
    "1;32", // light green
    "1;36", // light cyan
    "1;31", // light red
    "1;35", // light purple
    "1;33", // yellow
    "1;37", // white
};

void uart_set_color(uart_port_t port, uchar color)
{
    uart_puts(port, "\033[");
    uart_puts(port, (string)color_code[color]);
    uart_puts(port, "m");
}

void uart_puts(uart_port_t port, string str)
{
    for (u32 i = 0; str[i] != '\0'; i++)
    {
        uart_put(port, str[i]);
    }
}