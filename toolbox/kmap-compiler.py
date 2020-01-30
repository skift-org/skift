#!/usr/bin/python3

import json
import sys
import struct

KEYS = {
    "KEY_ESC": 0x01,
    "KEY_NUM1": 0x02,
    "KEY_NUM2": 0x03,
    "KEY_NUM3": 0x04,
    "KEY_NUM4": 0x05,
    "KEY_NUM5": 0x06,
    "KEY_NUM6": 0x07,
    "KEY_NUM7": 0x08,
    "KEY_NUM8": 0x09,
    "KEY_NUM9": 0x0A,
    "KEY_NUM0": 0x0B,
    "KEY_SYM1": 0x0C,
    "KEY_SYM2": 0x0D,
    "KEY_BKSPC": 0x0E,
    "KEY_TAB": 0x0F,
    "KEY_Q": 0x10,
    "KEY_W": 0x11,
    "KEY_E": 0x12,
    "KEY_R": 0x13,
    "KEY_T": 0x14,
    "KEY_Y": 0x15,
    "KEY_U": 0x16,
    "KEY_I": 0x17,
    "KEY_O": 0x18,
    "KEY_P": 0x19,
    "KEY_SYM3": 0x1A,
    "KEY_SYM4": 0x1B,
    "KEY_ENTER": 0x1C,
    "KEY_LCTRL": 0x1D,
    "KEY_A": 0x1E,
    "KEY_S": 0x1F,
    "KEY_D": 0x20,
    "KEY_F": 0x21,
    "KEY_G": 0x22,
    "KEY_H": 0x23,
    "KEY_J": 0x24,
    "KEY_K": 0x25,
    "KEY_L": 0x26,
    "KEY_SYM5": 0x27,
    "KEY_SYM6": 0x28,
    "KEY_SYM7": 0x29,
    "KEY_LSHIFT": 0x2A,
    "KEY_SYM8": 0x2B,
    "KEY_Z": 0x2C,
    "KEY_X": 0x2D,
    "KEY_C": 0x2E,
    "KEY_V": 0x2F,
    "KEY_B": 0x30,
    "KEY_N": 0x31,
    "KEY_M": 0x32,
    "KEY_SYM9": 0x33,
    "KEY_SYM10": 0x34,
    "KEY_SYM11": 0x35,
    "KEY_RSHIFT": 0x36,
    "KEY_SYM12": 0x37,
    "KEY_LALT": 0x38,
    "KEY_SPACE": 0x39,
    "KEY_CAPSLOCK": 0x3A,
    "KEY_F1": 0x3B,
    "KEY_F2": 0x3C,
    "KEY_F3": 0x3D,
    "KEY_F4": 0x3E,
    "KEY_F5": 0x3F,
    "KEY_F6": 0x40,
    "KEY_F7": 0x41,
    "KEY_F8": 0x42,
    "KEY_F9": 0x43,
    "KEY_F10": 0x44,
    "KEY_NUMLOCK": 0x45,
    "KEY_SCROLLLOCK": 0x46,
    "KEY_KPAD7": 0x47,
    "KEY_KPAD8": 0x48,
    "KEY_KPAD9": 0x49,
    "KEY_SYM13": 0x4A,
    "KEY_KPAD4": 0x4B,
    "KEY_KPAD5": 0x4C,
    "KEY_KPAD6": 0x4D,
    "KEY_SYM14": 0x4E,
    "KEY_KPAD1": 0x4F,
    "KEY_KPAD2": 0x50,
    "KEY_KPAD3": 0x51,
    "KEY_KPAD0": 0x52,
    "KEY_SYM15": 0x53,
    "KEY_ALTSYSRQ": 0x54,
    "KEY_NO_STANDARD_MEANING_1": 0x55,
    "KEY_NO_STANDARD_MEANING_2": 0x56,
    "KEY_F11": 0x57,
    "KEY_F12": 0x58,
    "KEY_KPADENTER": (0x80 + 0x1C),
    "KEY_RCTRL": (0x80 + 0x1D),
    "KEY_FAKELSHIFT": (0x80 + 0x2A),
    "KEY_SYM16": (0x80 + 0x35),
    "KEY_FAKERSHIFT": (0x80 + 0x36),
    "KEY_CTRLPRINTSCRN": (0x80 + 0x37),
    "KEY_RALT": (0x80 + 0x38),
    "KEY_CTRLBREAK": (0x80 + 0x46),
    "KEY_HOME": (0x80 + 0x47),
    "KEY_UP": (0x80 + 0x48),
    "KEY_PGUP": (0x80 + 0x49),
    "KEY_LEFT": (0x80 + 0x4B),
    "KEY_RIGHT": (0x80 + 0x4D),
    "KEY_END": (0x80 + 0x4F),
    "KEY_DOWN": (0x80 + 0x50),
    "KEY_PGDOWN": (0x80 + 0x51),
    "KEY_INSERT": (0x80 + 0x52),
    "KEY_DELETE": (0x80 + 0x53),
    "KEY_LSUPER": (0x80 + 0x5B),
    "KEY_RSUPER": (0x80 + 0x5C),
    "KEY_MENU": (0x80 + 0x5D),
}

in_filename = sys.argv[1]
out_filename = sys.argv[2]

infp = open(in_filename, 'r')
data = json.load(infp)
infp.close()

outfp = open(out_filename, 'wb')

outfp.write(struct.pack("4s", b"kmap"))
outfp.write(struct.pack("16s", data["language"].encode("utf-8")))
outfp.write(struct.pack("16s", data["region"].encode("utf-8")))

outfp.write(struct.pack("I", len(data["mappings"])))

for key in data["mappings"]:
    bind = data["mappings"][key]

    if len(bind) == 4:
        codepoint_regular = 0 if len(bind[0]) == 0 else ord(bind[0])
        codepoint_shift = 0 if len(bind[1]) == 0 else ord(bind[1])
        codepoint_alt = 0 if len(bind[2]) == 0 else ord(bind[2])
        codepoint_shift_alt = 0 if len(bind[3]) == 0 else ord(bind[3])

        outfp.write(struct.pack(
            "IIIII", KEYS[key], codepoint_regular, codepoint_shift, codepoint_alt, codepoint_shift_alt))
    else:
        print("Warning invalid mapping for key " + key)


outfp.close()
