#pragma once

//#include <libsystem/runtime.h>

typedef enum 
{
    KEY_MOTION_UP,
    KEY_MOTION_DOWN
} key_motion_t;

#define KEY_LIST(__ENTRY)                     \
    __ENTRY(KEY_ESC, 0x01)                    \
    __ENTRY(KEY_NUM1, 0x02)                   \
    __ENTRY(KEY_NUM2, 0x03)                   \
    __ENTRY(KEY_NUM3, 0x04)                   \
    __ENTRY(KEY_NUM4, 0x05)                   \
    __ENTRY(KEY_NUM5, 0x06)                   \
    __ENTRY(KEY_NUM6, 0x07)                   \
    __ENTRY(KEY_NUM7, 0x08)                   \
    __ENTRY(KEY_NUM8, 0x09)                   \
    __ENTRY(KEY_NUM9, 0x0A)                   \
    __ENTRY(KEY_NUM0, 0x0B)                   \
    __ENTRY(KEY_SYM1, 0x0C)                   \
    __ENTRY(KEY_SYM2, 0x0D)                   \
    __ENTRY(KEY_BKSPC, 0x0E)                  \
    __ENTRY(KEY_TAB, 0x0F)                    \
    __ENTRY(KEY_Q, 0x10)                      \
    __ENTRY(KEY_W, 0x11)                      \
    __ENTRY(KEY_E, 0x12)                      \
    __ENTRY(KEY_R, 0x13)                      \
    __ENTRY(KEY_T, 0x14)                      \
    __ENTRY(KEY_Y, 0x15)                      \
    __ENTRY(KEY_U, 0x16)                      \
    __ENTRY(KEY_I, 0x17)                      \
    __ENTRY(KEY_O, 0x18)                      \
    __ENTRY(KEY_P, 0x19)                      \
    __ENTRY(KEY_SYM3, 0x1A)                   \
    __ENTRY(KEY_SYM4, 0x1B)                   \
    __ENTRY(KEY_ENTER, 0x1C)                  \
    __ENTRY(KEY_LCTRL, 0x1D)                  \
    __ENTRY(KEY_A, 0x1E)                      \
    __ENTRY(KEY_S, 0x1F)                      \
    __ENTRY(KEY_D, 0x20)                      \
    __ENTRY(KEY_F, 0x21)                      \
    __ENTRY(KEY_G, 0x22)                      \
    __ENTRY(KEY_H, 0x23)                      \
    __ENTRY(KEY_J, 0x24)                      \
    __ENTRY(KEY_K, 0x25)                      \
    __ENTRY(KEY_L, 0x26)                      \
    __ENTRY(KEY_SYM5, 0x27)                   \
    __ENTRY(KEY_SYM6, 0x28)                   \
    __ENTRY(KEY_SYM7, 0x29)                   \
    __ENTRY(KEY_LSHIFT, 0x2A)                 \
    __ENTRY(KEY_SYM8, 0x2B)                   \
    __ENTRY(KEY_Z, 0x2C)                      \
    __ENTRY(KEY_X, 0x2D)                      \
    __ENTRY(KEY_C, 0x2E)                      \
    __ENTRY(KEY_V, 0x2F)                      \
    __ENTRY(KEY_B, 0x30)                      \
    __ENTRY(KEY_N, 0x31)                      \
    __ENTRY(KEY_M, 0x32)                      \
    __ENTRY(KEY_SYM9, 0x33)                   \
    __ENTRY(KEY_SYM10, 0x34)                  \
    __ENTRY(KEY_SYM11, 0x35)                  \
    __ENTRY(KEY_RSHIFT, 0x36)                 \
    __ENTRY(KEY_SYM12, 0x37)                  \
    __ENTRY(KEY_LALT, 0x38)                   \
    __ENTRY(KEY_SPACE, 0x39)                  \
    __ENTRY(KEY_CAPSLOCK, 0x3A)               \
    __ENTRY(KEY_F1, 0x3B)                     \
    __ENTRY(KEY_F2, 0x3C)                     \
    __ENTRY(KEY_F3, 0x3D)                     \
    __ENTRY(KEY_F4, 0x3E)                     \
    __ENTRY(KEY_F5, 0x3F)                     \
    __ENTRY(KEY_F6, 0x40)                     \
    __ENTRY(KEY_F7, 0x41)                     \
    __ENTRY(KEY_F8, 0x42)                     \
    __ENTRY(KEY_F9, 0x43)                     \
    __ENTRY(KEY_F10, 0x44)                    \
    __ENTRY(KEY_NUMLOCK, 0x45)                \
    __ENTRY(KEY_SCROLLLOCK, 0x46)             \
    __ENTRY(KEY_KPAD7, 0x47)                  \
    __ENTRY(KEY_KPAD8, 0x48)                  \
    __ENTRY(KEY_KPAD9, 0x49)                  \
    __ENTRY(KEY_SYM13, 0x4A)                  \
    __ENTRY(KEY_KPAD4, 0x4B)                  \
    __ENTRY(KEY_KPAD5, 0x4C)                  \
    __ENTRY(KEY_KPAD6, 0x4D)                  \
    __ENTRY(KEY_SYM14, 0x4E)                  \
    __ENTRY(KEY_KPAD1, 0x4F)                  \
    __ENTRY(KEY_KPAD2, 0x50)                  \
    __ENTRY(KEY_KPAD3, 0x51)                  \
    __ENTRY(KEY_KPAD0, 0x52)                  \
    __ENTRY(KEY_SYM15, 0x53)                  \
    __ENTRY(KEY_ALTSYSRQ, 0x54)               \
    __ENTRY(KEY_NO_STANDARD_MEANING_1, 0x55)  \
    __ENTRY(KEY_NO_STANDARD_MEANING_2, 0x56)  \
    __ENTRY(KEY_F11, 0x57)                    \
    __ENTRY(KEY_F12, 0x58)                    \
    __ENTRY(KEY_KPADENTER, (0x80 + 0x1C))     \
    __ENTRY(KEY_RCTRL, (0x80 + 0x1D))         \
    __ENTRY(KEY_FAKELSHIFT, (0x80 + 0x2A))    \
    __ENTRY(KEY_SYM16, (0x80 + 0x35))         \
    __ENTRY(KEY_FAKERSHIFT, (0x80 + 0x36))    \
    __ENTRY(KEY_CTRLPRINTSCRN, (0x80 + 0x37)) \
    __ENTRY(KEY_RALT, (0x80 + 0x38))          \
    __ENTRY(KEY_CTRLBREAK, (0x80 + 0x46))     \
    __ENTRY(KEY_HOME, (0x80 + 0x47))          \
    __ENTRY(KEY_UP, (0x80 + 0x48))            \
    __ENTRY(KEY_PGUP, (0x80 + 0x49))          \
    __ENTRY(KEY_LEFT, (0x80 + 0x4B))          \
    __ENTRY(KEY_RIGHT, (0x80 + 0x4D))         \
    __ENTRY(KEY_END, (0x80 + 0x4F))           \
    __ENTRY(KEY_DOWN, (0x80 + 0x50))          \
    __ENTRY(KEY_PGDOWN, (0x80 + 0x51))        \
    __ENTRY(KEY_INSERT, (0x80 + 0x52))        \
    __ENTRY(KEY_DELETE, (0x80 + 0x53))        \
    __ENTRY(KEY_LSUPER, (0x80 + 0x5B))        \
    __ENTRY(KEY_RSUPER, (0x80 + 0x5C))        \
    __ENTRY(KEY_MENU, (0x80 + 0x5D))

#define KEY_ENUM_ENTRY(__key_name, __key_number) __key_name = __key_number,

typedef enum
{
    KEY_LIST(KEY_ENUM_ENTRY)
    __KEY_COUNT,
} key_t;

const char *key_to_string(key_t key);

bool key_is_valid(key_t key);

