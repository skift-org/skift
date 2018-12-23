/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* plugs.c: Plugs functions for using the skift Framework in the kernel.      */

#include <string.h>
#include <skift/atomic.h>
#include <skift/logger.h>
#include <skift/formater.h>
#include <skift/__plugs.h>

#include "kernel/serial.h"
#include "kernel/system.h"
#include "kernel/memory.h"
#include "kernel/console.h"

void __plug_init(void)
{
    sk_formater_init();
}

int __plug_print(const char *buffer)
{
    console_print(buffer);
    serial_writeln((char *)buffer);
    return strlen(buffer);
}

void __plug_putchar(int c)
{
    serial_putc(c);
}

int __plug_getchar()
{
    sk_log(LOG_ERROR, "__plug_getchar() not implemented!");
    return EOF;
}

void __plug_read(char *buffer, uint size)
{
    UNUSED(buffer);
    UNUSED(size);

    sk_log(LOG_ERROR, "__plug_read() not implemented!");
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    sk_log(LOG_FATAL, "Kernel assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    PANIC("Kernel assert failed (see logs).");
}

int __plug_logger_lock()
{
    sk_atomic_begin();
    return 0;
}

int __plug_logger_unlock()
{
    sk_atomic_end();
    return 0;
}

int __plug_memalloc_lock()
{
    sk_atomic_begin();
    return 0;
}

int __plug_memalloc_unlock()
{
    sk_atomic_end();
    return 0;
}

void *__plug_memalloc_alloc(uint size)
{
    void *p = (void *)memory_alloc(memory_kpdir(), size, 0);
    return p;
}

int __plug_memalloc_free(void *memory, uint size)
{
    memory_free(memory_kpdir(), (uint)memory, size, 0);
    return 0;
}