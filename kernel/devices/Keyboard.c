
/* keyboard.c: keyboard driver                                                */

#include <libdevice/keyboard.h>
#include <libdevice/keymap.c>
#include <libdevice/keymap.h>
#include <libdevice/keys.c>
#include <libdevice/keys.h>
#include <libsystem/Result.h>
#include <libsystem/RingBuffer.h>
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

static PS2KeyboardState _state = PS2KBD_STATE_NORMAL;
static KeyMotion _keystate[__KEY_COUNT] = {KEY_MOTION_UP};
static KeyMap *_keymap = NULL;

static FsNode *_characters_node = NULL;
static RingBuffer *_characters_buffer = NULL;

static FsNode *_events_node = NULL;
static RingBuffer *_events_buffer = NULL;

Codepoint keyboard_get_codepoint(Key key)
{
    KeyMapping *mapping = keymap_lookup(_keymap, key);

    if (mapping == NULL)
    {
        return 0;
    }
    else
    {
        if (_keystate[KEY_LSHIFT] == KEY_MOTION_DOWN ||
            _keystate[KEY_RSHIFT] == KEY_MOTION_DOWN)
        {
            return mapping->shift_codepoint;
        }
        else if (_keystate[KEY_RALT] == KEY_MOTION_DOWN)
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
            if (keyboard_get_codepoint(key) != 0)
            {
                uint8_t utf8[5];
                int length = codepoint_to_utf8(keyboard_get_codepoint(key), utf8);

                if (_characters_node->readers)
                    ringbuffer_write(_characters_buffer, (const char *)utf8, length);
            }
        }

        if (_events_node->readers)
            ringbuffer_write(_events_buffer, (char *)&(KeyboardPacket){key, motion}, sizeof(KeyboardPacket));

        _keystate[key] = motion;
    }
    else
    {
        logger_warn("Invalid scancode %d", key);
    }
}

Key keyboard_scancode_to_key(int scancode)
{
#define SCAN_CODE_TO_KEY_ENTRY(__key_name, __key_number) \
    if (scancode == (__key_number))                      \
        return __key_name;

    KEY_LIST(SCAN_CODE_TO_KEY_ENTRY);

    return KEY_INVALID;
}

void keyboard_interrupt_handler(void)
{
    uint8_t scancode = in8(0x60);

    if (_state == PS2KBD_STATE_NORMAL)
    {
        if (scancode == PS2KBD_ESCAPE)
        {
            _state = PS2KBD_STATE_ESCAPED;
        }
        else
        {
            Key key = keyboard_scancode_to_key(scancode & 0x7F);
            keyboard_handle_key(key, scancode & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);
        }
    }
    else if (_state == PS2KBD_STATE_ESCAPED)
    {
        _state = PS2KBD_STATE_NORMAL;

        Key key = keyboard_scancode_to_key((scancode & 0x7F) + 0x80);
        keyboard_handle_key(key, scancode & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);
    }
}

/* --- Public functions ----------------------------------------------------- */

KeyMap *keyboard_load_keymap(const char *keymap_path)
{
    __cleanup(stream_cleanup) Stream *keymap_file = stream_open(keymap_path, OPEN_READ);

    if (handle_has_error(keymap_file))
    {
        logger_error("Failled to load keymap from %s: %s", keymap_path, handle_error_string(keymap_file));

        return NULL;
    }

    FileState stat;
    stream_stat(keymap_file, &stat);

    if (stat.type != FILE_TYPE_REGULAR)
    {
        logger_info("Failled to load keymap from %s: This is not a regular file", keymap_path);

        return NULL;
    }

    logger_info("Allocating keymap of size %dkio", stat.size / 1024);
    KeyMap *keymap = (KeyMap *)malloc(stat.size);

    size_t read = stream_read(keymap_file, keymap, stat.size);

    if (read != stat.size)
    {
        logger_error("Failled to load keymap from %s: %s", keymap_path, handle_error_string(keymap_file));

        free(keymap);

        return NULL;
    }

    return keymap;
}

static Result keyboard_iocall(FsNode *node, FsHandle *handle, int request, void *args)
{
    __unused(node);
    __unused(handle);

    if (request == KEYBOARD_CALL_SET_KEYMAP)
    {
        keyboard_set_keymap_args_t *size_and_keymap = (keyboard_set_keymap_args_t *)args;
        KeyMap *new_keymap = (KeyMap *)size_and_keymap->keymap;

        atomic_begin();

        if (_keymap != NULL)
        {
            free(_keymap);
        }

        _keymap = (KeyMap *)malloc(size_and_keymap->size);
        memcpy(_keymap, new_keymap, size_and_keymap->size);

        atomic_end();

        return SUCCESS;
    }
    else if (request == KEYBOARD_CALL_GET_KEYMAP)
    {
        memcpy(args, _keymap, sizeof(KeyMap));
        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}

static bool characters_can_read(FsNode *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(_characters_buffer);
}

static Result characters_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);

    // FIXME: use locks
    atomic_begin();
    *read = ringbuffer_read(_characters_buffer, (char *)buffer, size);
    atomic_end();

    return SUCCESS;
}

static bool events_can_read(FsNode *node, FsHandle *handle)
{
    __unused(node);
    __unused(handle);

    // FIXME: make this atomic or something...
    return !ringbuffer_is_empty(_events_buffer);
}

static Result events_read(FsNode *node, FsHandle *handle, void *buffer, size_t size, size_t *read)
{
    __unused(node);
    __unused(handle);

    // FIXME: use locks
    atomic_begin();
    *read = ringbuffer_read(_events_buffer, (char *)buffer, (size / sizeof(KeyboardPacket)) * sizeof(KeyboardPacket));
    atomic_end();

    return SUCCESS;
}

void keyboard_initialize(void)
{
    logger_info("Initializing keyboard...");

    _keymap = keyboard_load_keymap("/res/keyboard/fr_be.kmap");

    _characters_buffer = ringbuffer_create(1024);
    _characters_node = __create(FsNode);

    fsnode_init(_characters_node, FILE_TYPE_DEVICE);

    FSNODE(_characters_node)->call = (FsOperationCall)keyboard_iocall;
    FSNODE(_characters_node)->can_read = (FsOperationCanRead)characters_can_read;
    FSNODE(_characters_node)->read = (FsOperationRead)characters_read;

    filesystem_link_cstring("/dev/keyboard", _characters_node);

    _events_buffer = ringbuffer_create(1024);
    _events_node = __create(FsNode);

    fsnode_init(_events_node, FILE_TYPE_DEVICE);

    FSNODE(_events_node)->call = (FsOperationCall)keyboard_iocall;
    FSNODE(_events_node)->can_read = (FsOperationCanRead)events_can_read;
    FSNODE(_events_node)->read = (FsOperationRead)events_read;

    filesystem_link_cstring("/dev/keyboard-events", _events_node);

    dispatcher_register_handler(1, keyboard_interrupt_handler);
}
