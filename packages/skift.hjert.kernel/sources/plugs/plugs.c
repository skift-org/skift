/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* plugs.c: Plugs functions for using the skift Framework in the kernel.      */

#include <skift/__plugs__.h>

#include <skift/cstring.h>
#include <skift/atomic.h>
#include <skift/logger.h>
#include <skift/iostream.h>

#include "kernel/serial.h"
#include "kernel/system.h"
#include "kernel/memory.h"
#include "kernel/console.h"

iostream_t *in_stream ;
iostream_t *out_stream;
iostream_t *err_stream;
iostream_t *log_stream;

iostream_t internal_log_stream = {0};

int log_stream_write(iostream_t* stream, const void* buffer, uint size)
{
    UNUSED(stream);
    return serial_write(buffer, size);
}

void __plug_init(void)
{
    internal_log_stream.write = log_stream_write;

    in_stream  = NULL;
    out_stream = &internal_log_stream;
    err_stream = &internal_log_stream;
    log_stream = &internal_log_stream;

    sk_logger_setlevel(LOG_ERROR);
}

void __plug_assert_failed(const char *expr, const char *file, const char *function, int line)
{
    sk_log(LOG_FATAL, "Kernel assert failed: %s in %s:%s() ln%d!", (char *)expr, (char *)file, (char *)function, line);
    PANIC("Kernel assert failed (see logs).");
}


