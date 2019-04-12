/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* plugs.c: Plugs functions for using the skift Framework in the kernel.      */

#include <skift/__plugs__.h>

#include <string.h>
#include <skift/atomic.h>
#include <skift/logger.h>
#include <skift/formatter.h>
#include <skift/iostream.h>

#include "kernel/serial.h"
#include "kernel/system.h"
#include "kernel/memory.h"
#include "kernel/console.h"

void __plug_init(void)
{
    sk_formatter_init();
    sk_logger_setlevel(LOG_ERROR);
}

int __plug_print(const char *buffer)
{
    sk_atomic_begin();

    console_print(buffer);
    serial_writeln((char *)buffer);

    sk_atomic_end();

    return strlen(buffer);
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    sk_log(LOG_FATAL, "Kernel assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    PANIC("Kernel assert failed (see logs).");
}


