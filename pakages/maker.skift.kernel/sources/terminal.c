/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/console.h>
#include <skift/drawing.h>

#include "kernel/serial.h"

console_t * c;

void terminal_setup()
{
    c = console(80, 25);
}

void terminal_writeln(const char * message)
{
    console_writeln(c, message);
    serial_writeln(message);
}