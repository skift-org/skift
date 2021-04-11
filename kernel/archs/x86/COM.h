#pragma once

#include <libsystem/Common.h>

enum COMPort
{
    COM1 = 0x3F8,
    COM2 = 0x2F8,
    COM3 = 0x3E8,
    COM4 = 0x2E8,
};

bool com_can_read(COMPort port);

bool com_can_write(COMPort port);

void com_wait_write(COMPort port);

void com_wait_read(COMPort port);

void com_putc(COMPort port, char c);

char com_getc(COMPort port);

size_t com_write(COMPort port, const void *buffer, size_t size);

void com_initialize(COMPort port);
