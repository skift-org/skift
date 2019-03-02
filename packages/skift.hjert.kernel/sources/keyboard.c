/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* keyboard.c: keyboard driver                                                */

#include <skift/logger.h>
#include <skift/ascii.h>

#include "kernel/cpu/irq.h"
#include "kernel/keyboard.h"
#include "kernel/messaging.h"

/*
 * TODO:
 * - Send messages to subscribers
 */

char keymap_FRBE[128][3] = 
{
//   REG   MAJ   ALT
    {'\0', '\0', '\0'}, //  0 ERROR
    {'\0', '\0', '\0'}, //  1 esc
    { '&',  '1',  '|'}, //  2 NUM 1
    {'\0',  '2',  '@'}, //  3 NUM 2
    { '"',  '3',  '#'}, //  4 NUM 3
    {'\'',  '4',  '{'}, //  5 NUM 4
    { '(',  '5',  '['}, //  6 NUM 5
    {'\0',  '6',  '^'}, //  7 NUM 6
    {'\0',  '7', '\0'}, //  8 NUM 7
    { '!',  '8', '\0'}, //  9 NUM 8
    {'\0',  '9',  '{'}, // 10 NUM 9
    {'\0',  '0',  '}'}, // 11 NUM 0
    { ')', '\0',  ')'}, // 12 MINUX
    { '-',  '_',  '-'}, // 13 EQUAL
    {'\b', '\b', '\b'}, // 14 BACKSPACE
    {'\t', '\t', '\t'}, // 15 TAB
    
    { 'a',  'A', '\0'}, // 16 A
    { 'z',  'Z', '\0'}, // 17 Z
    { 'e',  'E', '\0'}, // 18 E
    { 'r',  'R', '\0'}, // 19 R
    { 't',  'T', '\0'}, // 20 T
    { 'y',  'Y', '\0'}, // 21 Y
    { 'u',  'U', '\0'}, // 22 U
    { 'i',  'I', '\0'}, // 23 I
    { 'o',  'O', '\0'}, // 24 O
    { 'p',  'P', '\0'}, // 25 P
    { '^',  '\0', '['}, // 26 [
    { '$',  '*',  ']'}, // 27 ]
    
    {'\n', '\n', '\n'}, // 28 ENTER
    {'\0', '\0', '\0'}, // 29 LCTRL

    { 'q',  'Q', '\0'}, // 30 Q
    { 's',  'S', '\0'}, // 31 S
    { 'd',  'D', '\0'}, // 32 D
    { 'f',  'F', '\0'}, // 33 F
    { 'g',  'G', '\0'}, // 34 G
    { 'h',  'H', '\0'}, // 35 H
    { 'j',  'J', '\0'}, // 36J
    { 'k',  'K', '\0'}, // 37 K
    { 'l',  'L', '\0'}, // 38 L
    { 'm',  'M', '\0'}, // 39 M
    {'\0',  '%', '\0'}, // 40 ù
    {'\0', '\0', '\0'}, // 41 ² ³ 
    {'\0', '\0', '\0'}, // 42 LSHIFT
    {'\0', '\0', ASCII_BACKTICK}, // 53 µ £ `
    { 'w',  'W', '\0'}, // 44 w 
    { 'x',  'X', '\0'}, // 45 x 
    { 'c',  'C', '\0'}, // 46 c 
    { 'v',  'V', '\0'}, // 47 v 
    { 'b',  'B', '\0'}, // 48 b 
    { 'n',  'N', '\0'}, // 49 n 
    { ',',  '?', '\0'}, // 50
    { ';',  '.', '\0'}, // 51
    { ':',  '/', '\0'}, // 52
    { '/',  '/',  '\0'}, // 53 KPDIV
    {'\0', '\0', '\0'}, // 54 RSHIFT
    { '*',  '*', '\0'}, // 55 KPMULT
    {'\0', '\0', '\0'}, // 56 LALT
    { ' ',  ' ',  ' '}, // 57 SPACE
    {'\0', '\0', '\0'}, // 58 CAPSLOCK
    {'\0', '\0', '\0'}, // 59 F1
    {'\0', '\0', '\0'}, // 60 F2
    {'\0', '\0', '\0'}, // 61 F3
    {'\0', '\0', '\0'}, // 62 F4
    {'\0', '\0', '\0'}, // 63 F5
    {'\0', '\0', '\0'}, // 64 F6
    {'\0', '\0', '\0'}, // 65 F7
    {'\0', '\0', '\0'}, // 66 F8
    {'\0', '\0', '\0'}, // 67 F9
    {'\0', '\0', '\0'}, // 68 F10
    {'\0', '\0', '\0'}, // 69 NUM_LOCK
    {'\0', '\0', '\0'}, // 70 SCROLL_LOCK
    { '7', '\0', '\0'}, // 71 KP7
    { '8', '\0', '\0'}, // 72 KP8
    { '9', '\0', '\0'}, // 73 KP9
    { '-',  '-', '\0'}, // 74 KP_MINUS
    { '4', '\0', '\0'}, // 75 KP4
    { '5', '\0', '\0'}, // 76 KP5
    { '6', '\0', '\0'}, // 77 KP6
    { '+',  '+', '\0'}, // 78 KP_PLUS
    { '1', '\0', '\0'}, // 79 KP1
    { '2', '\0', '\0'}, // 80 KP2
    { '3', '\0', '\0'}, // KP3
    { '0', '\0', '\0'}, // KP0
    { '.',  '.', '\0'}, // KP_POINT
    {'\0', '\0', '\0'}, // F11
    {'\0', '\0', '\0'}, // F12
    { '<', '>',  '\\'}, // ANGLE_BRACKET

    // SPECIAl KEY
    {'\0', '\0', '\0'}, // UP
    {'\0', '\0', '\0'}, // DOWN
    {'\0', '\0', '\0'}, // LEFT
    {'\0', '\0', '\0'}, // RIGHT
    
    {'\n', '\n', '\n'}, // KP_ENTER

    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
    {'\0', '\0', '\0'}, // 
};

/* --- Private functions ---------------------------------------------------- */

bool extended = false;
bool ispressed[256];

char keyboard_getchar(uchar scancode)
{
    if (ispressed[LSHIFT] || ispressed[RSHIFT])
    {
        return keymap_FRBE[scancode][1];
    }
    else if (ispressed[RALT])
    {
       return keymap_FRBE[scancode][2]; 
    }
    else
    {
        return keymap_FRBE[scancode][0]; 
    }
}

reg32_t keyboard_irq(reg32_t esp, processor_context_t *context)
{
    UNUSED(context);

    uchar scancode = inb(0x60);

    if (scancode < 128)
    {
        if (extended)
        {
            switch (scancode)
            {
                
            case 28: scancode = KPENTER;   break;
            case 29: scancode = RCONTROL;  break;
            case 56: scancode = RALT;      break;
            case 71: scancode = HOME;      break;
            case 72: scancode = UP;        break;
            case 73: scancode = PAGE_UP;   break;
            case 75: scancode = LEFT;      break;
            case 77: scancode = RIGHT;     break;
            case 79: scancode = END;       break;
            case 80: scancode = DOWN;      break;
            case 81: scancode = PAGE_DOWN; break;
            case 82: scancode = INSERT;    break;
            case 83: scancode = DELETE;    break;
            case 91: scancode = SUPER;     break;

            default: 
                break;
            }

            extended = false;
        }

        keyboard_event_t keyevent = {
            .c = keyboard_getchar(scancode),
            .key = scancode};

        if (!ispressed[scancode])
        {
            messaging_broadcast(KEYBOARD_CHANNEL, KEYBOARD_KEYPRESSED, &keyevent, sizeof(keyevent), 0);
            ispressed[scancode] = true;
        }

        messaging_broadcast(KEYBOARD_CHANNEL, KEYBOARD_KEYTYPED, &keyevent, sizeof(keyevent), 0);
    }    
    else if (scancode == 224)
    {
        extended = true;
    }
    else
    {
        if (extended)
        {

            switch (scancode)
            {

            case 156: scancode = KPENTER;   break;
            case 157: scancode = RCONTROL;  break;
            case 184: scancode = RALT;      break;
            case 199: scancode = HOME;      break;
            case 200: scancode = UP;        break;
            case 201: scancode = PAGE_UP;   break;
            case 203: scancode = LEFT;      break;
            case 205: scancode = RIGHT;     break;
            case 207: scancode = END;       break;
            case 208: scancode = DOWN;      break;
            case 209: scancode = PAGE_DOWN; break;
            case 210: scancode = INSERT;    break;
            case 211: scancode = DELETE;    break;
            case 216: scancode = SUPER;     break;
            
            default:
                break;
            }

            extended = false;
        }
        else
        {
            scancode -= 128;
        }


        keyboard_event_t keyevent = {
            .c = '\0',
            .key = scancode};

        messaging_broadcast(KEYBOARD_CHANNEL, KEYBOARD_KEYRELEASED, &keyevent, sizeof(keyevent), 0);
        ispressed[scancode] = false;
    }

    return esp;
}

/* --- Public functions ----------------------------------------------------- */

void keyboard_setup()
{
    irq_register(1, keyboard_irq);
}