#include "kernel/cpu/irq.h"
#include "kernel/keyboard.h"
#include "kernel/logger.h"

/* --- Private functions ---------------------------------------------------- */

esp_t keyboard_irq(esp_t esp, context_t *context)
{
    UNUSED(context);

    uchar scan_code = inb(0x60);
    log("Scancode: 0x%x.", scan_code);



    return esp;
}

/* --- Public functions ----------------------------------------------------- */

void keyboard_setup()
{
    irq_register(1, keyboard_irq);
}

void keyboard_load_keymap(keymap_t * keymap)
{
    UNUSED(keymap);
}