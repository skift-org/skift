/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* keyboard.c: keyboard driver                                                */

#include <libdevice/keyboard.h>
#include <libdevice/keymap.c>
#include <libdevice/keymap.h>
#include <libdevice/keys.c>
#include <libdevice/keys.h>
#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/atomic.h>
#include <libsystem/cstring.h>
#include <libsystem/logger.h>

#include "kernel/filesystem/Filesystem.h"
#include "kernel/interrupts/Dispatcher.h"

/* --- Private functions ---------------------------------------------------- */

#define PS2KBD_ESCAPE 0xE0

typedef enum
{
    PS2KBD_STATE_NORMAL,
    PS2KBD_STATE_ESCAPED,
} PS2KeyboardState;

static PS2KeyboardState keyboard_state = PS2KBD_STATE_NORMAL;
static KeyMotion keyboard_keystate[__KEY_COUNT] = {KEY_MOTION_UP};
static KeyMap *keyboard_keymap = NULL;
static RingBuffer *keyboard_buffer = NULL;

int keyboad_get_codepoint(Key key)
{
    KeyMapping *mapping = keymap_lookup(keyboard_keymap, key);

    if (mapping == NULL)
    {
        return 0;
    }
    else
    {
        if (keyboard_keystate[KEY_LSHIFT] == KEY_MOTION_DOWN ||
            keyboard_keystate[KEY_RSHIFT] == KEY_MOTION_DOWN)
        {
            return mapping->shift_codepoint;
        }
        else if (keyboard_keystate[KEY_RALT] == KEY_MOTION_DOWN)
        {
            return mapping->alt_codepoint;
        }
        else
        {
            return mapping->regular_codepoint;
        }
    }
}

void keyboard_handle_key(Key key, KeyMotion motion)
{
    if (key_is_valid(key))
    {
        if (motion == KEY_MOTION_DOWN)
        {
            if (keyboad_get_codepoint(key) != 0)
            {
                uint8_t utf8[5];
                int lenght = codepoint_to_utf8(keyboad_get_codepoint(key), utf8);
                ringbuffer_write(keyboard_buffer, (char *)utf8, lenght);
            }
        }

        keyboard_keystate[key] = motion;
    }
    else
    {
        logger_warn("Invalide scancode %d", key);
    }
}

void keyboard_interrupt_handler(void)
{
    int byte = in8(0x60);

    if (keyboard_state == PS2KBD_STATE_NORMAL)
    {
        if (byte == PS2KBD_ESCAPE)
        {
            keyboard_state = PS2KBD_STATE_ESCAPED;
        }
        else
        {
            Key key = byte & 0x7F;
            keyboard_handle_key(key, byte & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);
        }
    }
    else if (keyboard_state == PS2KBD_STATE_ESCAPED)
    {
        keyboard_state = PS2KBD_STATE_NORMAL;

        Key key = (byte & 0x7F) + 0x80;
        keyboard_handle_key(key, byte & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);
    }
}

/* --- Public functions ----------------------------------------------------- */

KeyMap *keyboard_load_keymap(const char *keymap_path)
{
    Stream *keymap_file = stream_open(keymap_path, OPEN_READ);

    if (handle_has_error(keymap_file))
    {
        logger_error("Failled to load keymap from %s: %s", keymap_path, handle_error_string(keymap_file));
        stream_close(keymap_file);

        return NULL;
    }

    FileState stat;
    stream_stat(keymap_file, &stat);

    if (stat.type != FILE_TYPE_REGULAR)
    {
        logger_info("Failled to load keymap from %s: This is not a regular file", keymap_path);
        stream_close(keymap_file);

        return NULL;
    }

    logger_info("Allocating keymap of size %dkio", stat.size / 1024);
    KeyMap *keymap = malloc(stat.size);

    size_t readed = stream_read(keymap_file, keymap, stat.size);

    if (readed != stat.size)
    {
        logger_error("Failled to load keymap from %s: %s", keymap_path, handle_error_string(keymap_file));
        stream_close(keymap_file);

        free(keymap);

        return NULL;
    }

    stream_close(keymap_file);

    return keymap;
}

bool keyboard_FsOperationCanRead(FsNode *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(keyboard_buffer);
}

static Result keyboard_FsOperationRead(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *readed)
{
    __unused(node);
    __unused(handle);

    // FIXME: use locks
    atomic_begin();
    *readed = ringbuffer_read(keyboard_buffer, buffer, size);
    atomic_end();

    return SUCCESS;
}

Result keyboard_FsOperationCall(FsNode *node, FsHandle *handle, int request, void *args)
{
    __unused(node);
    __unused(handle);

    if (request == KEYBOARD_CALL_SET_KEYMAP)
    {
        keyboard_set_keymap_args_t *size_and_keymap = args;
        KeyMap *new_keymap = size_and_keymap->keymap;

        atomic_begin();

        if (keyboard_keymap != NULL)
        {
            free(keyboard_keymap);
        }

        keyboard_keymap = malloc(size_and_keymap->size);
        memcpy(keyboard_keymap, new_keymap, size_and_keymap->size);

        atomic_end();

        return SUCCESS;
    }
    else if (request == KEYBOARD_CALL_GET_KEYMAP)
    {
        if (keyboard_keymap != NULL)
        {
            memcpy(args, keyboard_keymap, sizeof(KeyMap));

            return SUCCESS;
        }
        else
        {
            // FIXME: Maybe add another ERR_* for this error...
            return ERR_INPUTOUTPUT_ERROR;
        }
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

void keyboard_initialize()
{
    logger_info("Initializing keyboad...");

    keyboard_keymap = keyboard_load_keymap("/res/keyboard/fr_fr.kmap");
    keyboard_buffer = ringbuffer_create(1024);

    dispatcher_register_handler(1, keyboard_interrupt_handler);

    FsNode *keyboard_device = __create(FsNode);

    fsnode_init(keyboard_device, FSNODE_DEVICE);

    FSNODE(keyboard_device)->call = (FsOperationCall)keyboard_FsOperationCall;
    FSNODE(keyboard_device)->read = (FsOperationRead)keyboard_FsOperationRead;
    FSNODE(keyboard_device)->can_read = (FsOperationCanRead)keyboard_FsOperationCanRead;

    Path *keyboard_device_path = path_create(KEYBOARD_DEVICE);
    filesystem_link_and_take_ref(keyboard_device_path, keyboard_device);
    path_destroy(keyboard_device_path);
}