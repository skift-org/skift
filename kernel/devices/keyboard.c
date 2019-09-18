/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* keyboard.c: keyboard driver                                                */

#include <libsystem/logger.h>
#include <libsystem/ringbuffer.h>
#include <libsystem/atomic.h>
#include <libsystem/error.h>

#include <libdevice/keyboard.h>
#include <libdevice/keys.h>
#include <libdevice/keys.c>
#include <libdevice/keymap.h>
#include <libdevice/keymap.c>
#include <libkernel/message.h>

#include "x86/irq.h"
#include "filesystem.h"
#include "tasking.h"

/* --- Private functions ---------------------------------------------------- */

#define PS2KBD_ESCAPE 0xE0

typedef enum
{
    PS2KBD_STATE_NORMAL,
    PS2KBD_STATE_NORMAL_ESCAPED,
} ps2_keyboard_state_t;

static ps2_keyboard_state_t keyboard_state = PS2KBD_STATE_NORMAL;
static key_motion_t keyboard_keystate[__KEY_COUNT] = {KEY_MOTION_UP};
static keymap_t *keyboard_keymap = NULL;

int keyboad_get_codepoint(key_t key)
{
    keymap_keybing_t *binding = keymap_lookup(keyboard_keymap, key);

    if (binding == NULL)
    {
        return 0;
    }
    else
    {
        if (keyboard_keystate[KEY_LSHIFT] == KEY_MOTION_DOWN ||
            keyboard_keystate[KEY_RSHIFT] == KEY_MOTION_DOWN)
        {
            return binding->shift_codepoint;
        }
        else if (keyboard_keystate[KEY_RALT] == KEY_MOTION_DOWN)
        {
            return binding->alt_codepoint;
        }
        else
        {
            return binding->regular_codepoint;
        }
    }
}

void keyboard_handle_key(key_t key, key_motion_t motion)
{
    if (key_is_valid(key))
    {
        // keymap_keybing_t *binding = keymap_lookup(keyboard_keymap, key);

        // if (binding != NULL)
        // {
        //     logger_debug("scancode %s %d: %s '%c'", motion == KEY_MOTION_UP ? "up" : "down", key, key_to_string(key), binding->regular_codepoint);
        // }
        // else
        // {
        //     logger_debug("scancode %s %d: %s", motion == KEY_MOTION_UP ? "up" : "down", key, key_to_string(key));
        // }

        if (motion == KEY_MOTION_DOWN)
        {
            if (keyboard_keystate[key] == KEY_MOTION_UP)
            {
                keyboard_event_t keyevent = {key, keyboad_get_codepoint(key)};
                message_t keypressed_event = message(KEYBOARD_KEYPRESSED, -1);
                message_set_payload(keypressed_event, keyevent);

                task_messaging_broadcast(task_kernel(), KEYBOARD_CHANNEL, &keypressed_event);
            }

            keyboard_event_t keyevent = {key, keyboad_get_codepoint(key)};
            message_t keypressed_event = message(KEYBOARD_KEYTYPED, -1);
            message_set_payload(keypressed_event, keyevent);

            task_messaging_broadcast(task_kernel(), KEYBOARD_CHANNEL, &keypressed_event);
        }
        else if (motion == KEY_MOTION_UP)
        {
            keyboard_event_t keyevent = {key, keyboad_get_codepoint(key)};
            message_t keypressed_event = message(KEYBOARD_KEYRELEASED, -1);
            message_set_payload(keypressed_event, keyevent);

            task_messaging_broadcast(task_kernel(), KEYBOARD_CHANNEL, &keypressed_event);
        }
        
        keyboard_keystate[key] = motion;
    }
    else
    {
        logger_warn("Invalide scancode %d", key);
    }
}

reg32_t keyboard_irq(reg32_t esp, processor_context_t *context)
{
    UNUSED(context);

    int byte = in8(0x60);

    if (keyboard_state == PS2KBD_STATE_NORMAL)
    {
        if (byte == PS2KBD_ESCAPE)
        {
            keyboard_state = PS2KBD_STATE_NORMAL_ESCAPED;
        }
        else
        {
            key_t key = byte & 0x7F;
            keyboard_handle_key(key, byte & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);
        }
    }
    else if (keyboard_state == PS2KBD_STATE_NORMAL_ESCAPED)
    {
        keyboard_state = PS2KBD_STATE_NORMAL;

        key_t key = (byte & 0x7F) + 0x80;
        keyboard_handle_key(key, byte & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);
    }

    return esp;
}

/* --- Public functions ----------------------------------------------------- */

keymap_t *keyboard_load_keymap(const char *path)
{
    iostream_t *kmfile = iostream_open(path, IOSTREAM_READ);

    if (kmfile == NULL)
    {
        return NULL;
    }

    iostream_stat_t stat;
    iostream_stat(kmfile, &stat);

    logger_info("Allocating keymap of size %dkio", stat.size / 1024);
    keymap_t *keymap = malloc(stat.size);

    iostream_read(kmfile, keymap, stat.size);

    iostream_close(kmfile);

    return keymap;
}

int keyboard_device_call(stream_t *stream, int request, void *args)
{
    UNUSED(stream);

    if (request == FRAMEBUFFER_CALL_SET_KEYMAP)
    {
        keyboard_set_keymap_args_t *size_and_keymap = args;
        keymap_t *new_keymap = size_and_keymap->keymap;

        atomic_begin();

        if (keyboard_keymap != NULL)
        {
            free(keyboard_keymap);
        }

        keyboard_keymap = malloc(size_and_keymap->size);
        memcpy(keyboard_keymap, new_keymap, size_and_keymap->size);

        atomic_end();

        return -ERR_SUCCESS;
    }
    else if (request == FRAMEBUFFER_CALL_GET_KEYMAP)
    {
        if (keyboard_keymap != NULL)
        {
            memcpy(args, keyboard_keymap, sizeof(keymap_t));

            return -ERR_SUCCESS;
        }
        else
        {
            // FIXME: Maybe add another ERR_* for this error...
            return -ERR_INPUTOUTPUT_ERROR;
        }
    }
    else
    {
        return -ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

void keyboard_setup()
{
    keyboard_keymap = keyboard_load_keymap("/res/keyboard/en_us.kmap");

    irq_register(1, keyboard_irq);

    device_t keyboard_device = {.call = keyboard_device_call};
    FILESYSTEM_MKDEV(KEYBOARD_DEVICE, keyboard_device);
}