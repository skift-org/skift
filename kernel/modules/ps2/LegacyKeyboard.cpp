#include <libsystem/io/Stream.h>

#include "system/Configs.h"
#include "system/Streams.h"

#include "ps2/LegacyKeyboard.h"

KeyMap *keyboard_load_keymap(const char *keymap_path)
{
    CLEANUP(stream_cleanup)
    Stream *keymap_file = stream_open(keymap_path, HJ_OPEN_READ);

    if (handle_has_error(keymap_file))
    {
        Kernel::logln("Failed to load keymap from {}: {}", keymap_path, handle_error_string(keymap_file));
        return nullptr;
    }

    HjStat stat;
    stream_stat(keymap_file, &stat);

    if (stat.type != HJ_FILE_TYPE_REGULAR)
    {
        Kernel::logln("Failed to load keymap from {}: This is not a regular file", keymap_path);

        return nullptr;
    }

    Kernel::logln("Allocating keymap of size {}kio", stat.size / 1024);
    KeyMap *keymap = (KeyMap *)malloc(stat.size);

    size_t read = stream_read(keymap_file, keymap, stat.size);

    if (read != stat.size)
    {
        Kernel::logln("Failed to load keymap from {}: {}", keymap_path, handle_error_string(keymap_file));

        free(keymap);

        return nullptr;
    }

    return keymap;
}

Key LegacyKeyboard::scancode_to_key(int scancode)
{
#define SCAN_CODE_TO_KEY_ENTRY(__key_name, __key_number) \
    if (scancode == (__key_number))                      \
        return __key_name;

    KEY_LIST(SCAN_CODE_TO_KEY_ENTRY);

    return KEYBOARD_KEY_INVALID;
}

Text::Rune LegacyKeyboard::key_to_rune(Key key)
{
    if (!_keymap)
    {
        Kernel::logln("No keymap loaded!");
        return 0;
    }

    KeyMapping *mapping = keymap_lookup(_keymap, key);

    if (mapping == nullptr)
    {
        return 0;
    }

    if (_keystate[KEYBOARD_KEY_LSHIFT] == KEY_MOTION_DOWN ||
        _keystate[KEYBOARD_KEY_RSHIFT] == KEY_MOTION_DOWN)
    {
        return mapping->shift_rune;
    }
    else if (_keystate[KEYBOARD_KEY_RALT] == KEY_MOTION_DOWN)
    {
        return mapping->alt_rune;
    }
    else
    {
        return mapping->regular_rune;
    }
}

KeyModifier LegacyKeyboard::modifiers()
{
    KeyModifier modifiers = 0;

    if (_keystate[KEYBOARD_KEY_LALT] == KEY_MOTION_DOWN)
    {
        modifiers |= KEY_MODIFIER_ALT;
    }

    if (_keystate[KEYBOARD_KEY_RALT] == KEY_MOTION_DOWN)
    {
        modifiers |= KEY_MODIFIER_ALTGR;
    }

    if (_keystate[KEYBOARD_KEY_LSHIFT] == KEY_MOTION_DOWN ||
        _keystate[KEYBOARD_KEY_RSHIFT] == KEY_MOTION_DOWN)
    {
        modifiers |= KEY_MODIFIER_SHIFT;
    }

    if (_keystate[KEYBOARD_KEY_LCTRL] == KEY_MOTION_DOWN ||
        _keystate[KEYBOARD_KEY_RCTRL] == KEY_MOTION_DOWN)
    {
        modifiers |= KEY_MODIFIER_CTRL;
    }

    if (_keystate[KEYBOARD_KEY_LSUPER] == KEY_MOTION_DOWN ||
        _keystate[KEYBOARD_KEY_RSUPER] == KEY_MOTION_DOWN)
    {
        modifiers |= KEY_MODIFIER_SUPER;
    }

    return modifiers;
}

void LegacyKeyboard::handle_key(Key key, KeyMotion motion)
{
    if (!key_is_valid(key))
    {
        Kernel::logln("Invalid scancode {}", key);
        return;
    }

    Text::Rune rune = key_to_rune(key);

    if (_keystate[key] == KEY_MOTION_UP && motion == KEY_MOTION_DOWN)
    {
        KeyboardPacket packet = {
            key,
            modifiers(),
            rune,
            KEY_MOTION_DOWN,
        };

        _events.write((char *)&packet, sizeof(KeyboardPacket));
    }

    if (motion == KEY_MOTION_UP)
    {
        KeyboardPacket packet = {
            key,
            modifiers(),
            rune,
            KEY_MOTION_UP,
        };

        _events.write((char *)&packet, sizeof(KeyboardPacket));
    }

    if (motion == KEY_MOTION_DOWN)
    {
        KeyboardPacket packet = {
            key,
            modifiers(),
            rune,
            KEY_MOTION_TYPED,
        };

        _events.write((char *)&packet, sizeof(KeyboardPacket));
    }

    _keystate[key] = motion;
}

LegacyKeyboard::LegacyKeyboard(DeviceAddress address) : LegacyDevice(address, DeviceClass::KEYBOARD)
{
    _keymap = keyboard_load_keymap("/Files/Keyboards/" CONFIG_KEYBOARD_LAYOUT ".kmap");
}

void LegacyKeyboard::handle_interrupt()
{
    uint8_t status = in8(PS2_STATUS);

    while (((status & PS2_WHICH_BUFFER) == PS2_KEYBOARD_BUFFER) &&
           (status & PS2_BUFFER_FULL))
    {
        uint8_t scancode = in8(PS2_BUFFER);

        if (_escaped)
        {
            Key key = scancode_to_key((scancode & 0x7F) + 0x80);
            handle_key(key, scancode & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);

            _escaped = false;
        }
        else
        {
            if (scancode == PS2KBD_ESCAPE)
            {
                _escaped = true;
            }
            else
            {
                Key key = scancode_to_key(scancode & 0x7F);
                handle_key(key, scancode & 0x80 ? KEY_MOTION_UP : KEY_MOTION_DOWN);
            }
        }

        status = in8(PS2_STATUS);
    }
}

bool LegacyKeyboard::can_read()
{
    return !_events.empty();
}

ResultOr<size_t> LegacyKeyboard::read(size64_t offset, void *buffer, size_t size)
{
    UNUSED(offset);

    return _events.read((char *)buffer, (size / sizeof(KeyboardPacket)) * sizeof(KeyboardPacket));
}

HjResult LegacyKeyboard::call(IOCall request, void *args)
{
    if (request == IOCALL_KEYBOARD_GET_KEYMAP)
    {
        memcpy(args, _keymap, sizeof(KeyMap));
        return SUCCESS;
    }
    else if (request == IOCALL_KEYBOARD_SET_KEYMAP)
    {
        auto set_keymap = (IOCallKeyboardSetKeymapArgs *)args;

        free(_keymap);
        _keymap = (KeyMap *)malloc(set_keymap->size);
        memcpy(_keymap, set_keymap->keymap, set_keymap->size);

        return SUCCESS;
    }
    else
    {
        return ERR_INAPPROPRIATE_CALL_FOR_DEVICE;
    }
}