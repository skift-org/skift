#pragma once
#include "types.h"

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

typedef unsigned short uart_port_t;

void uart_setup();
void uart_open(uart_port_t port);
void uart_put(uart_port_t port, char a);
void uart_puts(uart_port_t port, string str);
void uart_set_color(uart_port_t port, uchar color);