#pragma once

#include <libutils/unicode/Codepoint.h>

#define KEY_LIST(__ENTRY)                              \
    __ENTRY(KEYBOARD_KEY_INVALID, 0x0)                 \
    __ENTRY(KEYBOARD_KEY_ESC, 0x01)                    \
    __ENTRY(KEYBOARD_KEY_NUM1, 0x02)                   \
    __ENTRY(KEYBOARD_KEY_NUM2, 0x03)                   \
    __ENTRY(KEYBOARD_KEY_NUM3, 0x04)                   \
    __ENTRY(KEYBOARD_KEY_NUM4, 0x05)                   \
    __ENTRY(KEYBOARD_KEY_NUM5, 0x06)                   \
    __ENTRY(KEYBOARD_KEY_NUM6, 0x07)                   \
    __ENTRY(KEYBOARD_KEY_NUM7, 0x08)                   \
    __ENTRY(KEYBOARD_KEY_NUM8, 0x09)                   \
    __ENTRY(KEYBOARD_KEY_NUM9, 0x0A)                   \
    __ENTRY(KEYBOARD_KEY_NUM0, 0x0B)                   \
    __ENTRY(KEYBOARD_KEY_SYM1, 0x0C)                   \
    __ENTRY(KEYBOARD_KEY_SYM2, 0x0D)                   \
    __ENTRY(KEYBOARD_KEY_BKSPC, 0x0E)                  \
    __ENTRY(KEYBOARD_KEY_TAB, 0x0F)                    \
    __ENTRY(KEYBOARD_KEY_Q, 0x10)                      \
    __ENTRY(KEYBOARD_KEY_W, 0x11)                      \
    __ENTRY(KEYBOARD_KEY_E, 0x12)                      \
    __ENTRY(KEYBOARD_KEY_R, 0x13)                      \
    __ENTRY(KEYBOARD_KEY_T, 0x14)                      \
    __ENTRY(KEYBOARD_KEY_Y, 0x15)                      \
    __ENTRY(KEYBOARD_KEY_U, 0x16)                      \
    __ENTRY(KEYBOARD_KEY_I, 0x17)                      \
    __ENTRY(KEYBOARD_KEY_O, 0x18)                      \
    __ENTRY(KEYBOARD_KEY_P, 0x19)                      \
    __ENTRY(KEYBOARD_KEY_SYM3, 0x1A)                   \
    __ENTRY(KEYBOARD_KEY_SYM4, 0x1B)                   \
    __ENTRY(KEYBOARD_KEY_ENTER, 0x1C)                  \
    __ENTRY(KEYBOARD_KEY_LCTRL, 0x1D)                  \
    __ENTRY(KEYBOARD_KEY_A, 0x1E)                      \
    __ENTRY(KEYBOARD_KEY_S, 0x1F)                      \
    __ENTRY(KEYBOARD_KEY_D, 0x20)                      \
    __ENTRY(KEYBOARD_KEY_F, 0x21)                      \
    __ENTRY(KEYBOARD_KEY_G, 0x22)                      \
    __ENTRY(KEYBOARD_KEY_H, 0x23)                      \
    __ENTRY(KEYBOARD_KEY_J, 0x24)                      \
    __ENTRY(KEYBOARD_KEY_K, 0x25)                      \
    __ENTRY(KEYBOARD_KEY_L, 0x26)                      \
    __ENTRY(KEYBOARD_KEY_SYM5, 0x27)                   \
    __ENTRY(KEYBOARD_KEY_SYM6, 0x28)                   \
    __ENTRY(KEYBOARD_KEY_SYM7, 0x29)                   \
    __ENTRY(KEYBOARD_KEY_LSHIFT, 0x2A)                 \
    __ENTRY(KEYBOARD_KEY_SYM8, 0x2B)                   \
    __ENTRY(KEYBOARD_KEY_Z, 0x2C)                      \
    __ENTRY(KEYBOARD_KEY_X, 0x2D)                      \
    __ENTRY(KEYBOARD_KEY_C, 0x2E)                      \
    __ENTRY(KEYBOARD_KEY_V, 0x2F)                      \
    __ENTRY(KEYBOARD_KEY_B, 0x30)                      \
    __ENTRY(KEYBOARD_KEY_N, 0x31)                      \
    __ENTRY(KEYBOARD_KEY_M, 0x32)                      \
    __ENTRY(KEYBOARD_KEY_SYM9, 0x33)                   \
    __ENTRY(KEYBOARD_KEY_SYM10, 0x34)                  \
    __ENTRY(KEYBOARD_KEY_SYM11, 0x35)                  \
    __ENTRY(KEYBOARD_KEY_RSHIFT, 0x36)                 \
    __ENTRY(KEYBOARD_KEY_SYM12, 0x37)                  \
    __ENTRY(KEYBOARD_KEY_LALT, 0x38)                   \
    __ENTRY(KEYBOARD_KEY_SPACE, 0x39)                  \
    __ENTRY(KEYBOARD_KEY_CAPSLOCK, 0x3A)               \
    __ENTRY(KEYBOARD_KEY_F1, 0x3B)                     \
    __ENTRY(KEYBOARD_KEY_F2, 0x3C)                     \
    __ENTRY(KEYBOARD_KEY_F3, 0x3D)                     \
    __ENTRY(KEYBOARD_KEY_F4, 0x3E)                     \
    __ENTRY(KEYBOARD_KEY_F5, 0x3F)                     \
    __ENTRY(KEYBOARD_KEY_F6, 0x40)                     \
    __ENTRY(KEYBOARD_KEY_F7, 0x41)                     \
    __ENTRY(KEYBOARD_KEY_F8, 0x42)                     \
    __ENTRY(KEYBOARD_KEY_F9, 0x43)                     \
    __ENTRY(KEYBOARD_KEY_F10, 0x44)                    \
    __ENTRY(KEYBOARD_KEY_NUMLOCK, 0x45)                \
    __ENTRY(KEYBOARD_KEY_SCROLLLOCK, 0x46)             \
    __ENTRY(KEYBOARD_KEY_KPAD7, 0x47)                  \
    __ENTRY(KEYBOARD_KEY_KPAD8, 0x48)                  \
    __ENTRY(KEYBOARD_KEY_KPAD9, 0x49)                  \
    __ENTRY(KEYBOARD_KEY_SYM13, 0x4A)                  \
    __ENTRY(KEYBOARD_KEY_KPAD4, 0x4B)                  \
    __ENTRY(KEYBOARD_KEY_KPAD5, 0x4C)                  \
    __ENTRY(KEYBOARD_KEY_KPAD6, 0x4D)                  \
    __ENTRY(KEYBOARD_KEY_SYM14, 0x4E)                  \
    __ENTRY(KEYBOARD_KEY_KPAD1, 0x4F)                  \
    __ENTRY(KEYBOARD_KEY_KPAD2, 0x50)                  \
    __ENTRY(KEYBOARD_KEY_KPAD3, 0x51)                  \
    __ENTRY(KEYBOARD_KEY_KPAD0, 0x52)                  \
    __ENTRY(KEYBOARD_KEY_SYM15, 0x53)                  \
    __ENTRY(KEYBOARD_KEY_ALTSYSRQ, 0x54)               \
    __ENTRY(KEYBOARD_KEY_NO_STANDARD_MEANING_1, 0x55)  \
    __ENTRY(KEYBOARD_KEY_NO_STANDARD_MEANING_2, 0x56)  \
    __ENTRY(KEYBOARD_KEY_F11, 0x57)                    \
    __ENTRY(KEYBOARD_KEY_F12, 0x58)                    \
    __ENTRY(KEYBOARD_KEY_KPADENTER, (0x80 + 0x1C))     \
    __ENTRY(KEYBOARD_KEY_RCTRL, (0x80 + 0x1D))         \
    __ENTRY(KEYBOARD_KEY_FAKELSHIFT, (0x80 + 0x2A))    \
    __ENTRY(KEYBOARD_KEY_SYM16, (0x80 + 0x35))         \
    __ENTRY(KEYBOARD_KEY_FAKERSHIFT, (0x80 + 0x36))    \
    __ENTRY(KEYBOARD_KEY_CTRLPRINTSCRN, (0x80 + 0x37)) \
    __ENTRY(KEYBOARD_KEY_RALT, (0x80 + 0x38))          \
    __ENTRY(KEYBOARD_KEY_CTRLBREAK, (0x80 + 0x46))     \
    __ENTRY(KEYBOARD_KEY_HOME, (0x80 + 0x47))          \
    __ENTRY(KEYBOARD_KEY_UP, (0x80 + 0x48))            \
    __ENTRY(KEYBOARD_KEY_PGUP, (0x80 + 0x49))          \
    __ENTRY(KEYBOARD_KEY_LEFT, (0x80 + 0x4B))          \
    __ENTRY(KEYBOARD_KEY_RIGHT, (0x80 + 0x4D))         \
    __ENTRY(KEYBOARD_KEY_END, (0x80 + 0x4F))           \
    __ENTRY(KEYBOARD_KEY_DOWN, (0x80 + 0x50))          \
    __ENTRY(KEYBOARD_KEY_PGDOWN, (0x80 + 0x51))        \
    __ENTRY(KEYBOARD_KEY_INSERT, (0x80 + 0x52))        \
    __ENTRY(KEYBOARD_KEY_DELETE, (0x80 + 0x53))        \
    __ENTRY(KEYBOARD_KEY_LSUPER, (0x80 + 0x5B))        \
    __ENTRY(KEYBOARD_KEY_RSUPER, (0x80 + 0x5C))        \
    __ENTRY(KEYBOARD_KEY_MENU, (0x80 + 0x5D))

#define KEY_ENUM_ENTRY(__key_name, __key_number) __key_name,

enum Key
{
    KEY_LIST(KEY_ENUM_ENTRY)
        __KEY_COUNT,
};

enum KeyMotion
{
    KEY_MOTION_UP,
    KEY_MOTION_DOWN,
    KEY_MOTION_TYPED,
};

#define KEY_MODIFIER_ALT (1 << 0)
#define KEY_MODIFIER_ALTGR (1 << 1)
#define KEY_MODIFIER_SHIFT (1 << 2)
#define KEY_MODIFIER_CTRL (1 << 3)
#define KEY_MODIFIER_SUPER (1 << 3)
typedef unsigned int KeyModifier;

struct KeyboardPacket
{
    Key key;
    KeyModifier modifiers;
    Codepoint codepoint;
    KeyMotion motion;
};

struct KeyMapping
{
    Key key;

    Codepoint regular_codepoint;
    Codepoint shift_codepoint;
    Codepoint alt_codepoint;
    Codepoint shift_alt_codepoint;
};

#define KEYMAP_LANGUAGE_SIZE 16
#define KEYMAP_REGION_SIZE 16

struct KeyMap
{
    char magic[4]; /* kmap */
    char language[KEYMAP_LANGUAGE_SIZE];
    char region[KEYMAP_REGION_SIZE];

    size_t length;
    KeyMapping mappings[];
};

#define KEY_NAMES_ENTRY(__key_name, __key_number) #__key_name,

static const char *KEYS_NAMES[] = {KEY_LIST(KEY_NAMES_ENTRY)};

static inline const char *key_to_string(Key key)
{
    if (key < __KEY_COUNT)
    {
        return KEYS_NAMES[key];
    }
    else
    {
        return "overflow";
    }
}

static inline bool key_is_valid(Key key)
{
    return key > 0 && key < __KEY_COUNT;
}

static inline KeyMapping *keymap_lookup(KeyMap *keymap, Key key)
{
    for (size_t i = 0; i < keymap->length; i++)
    {
        if (keymap->mappings[i].key == key)
        {
            return &keymap->mappings[i];
        }
    }

    return nullptr;
}
