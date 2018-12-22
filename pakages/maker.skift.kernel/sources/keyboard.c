/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/logger.h>

#include "kernel/cpu/irq.h"
#include "kernel/keyboard.h"

/* --- Private functions ---------------------------------------------------- */

reg32_t keyboard_irq(reg32_t esp, context_t *context)
{
    UNUSED(context);

    uchar scan_code = inb(0x60);
    sk_log(LOG_DEBUG, "Scancode: 0x%x.", scan_code);

    return esp;
}

/* --- Public functions ----------------------------------------------------- */

void keyboard_setup()
{
    irq_register(1, keyboard_irq);
}