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

    uchar scancode = inb(0x60);

    if (scancode < 128)
    {
        sk_log(LOG_DEBUG, "down: %d.", scancode);
    }
    else if (scancode == 224)
    {
        sk_log(LOG_DEBUG, "Meta");
    }
    else
    {
        sk_log(LOG_DEBUG, "up: %d.", scancode);
        printf("\n");
    }

    return esp;
}

/* --- Public functions ----------------------------------------------------- */

void keyboard_setup()
{
    irq_register(1, keyboard_irq);
}