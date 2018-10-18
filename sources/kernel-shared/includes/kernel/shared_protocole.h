#pragma once

#include <skift/types.h>

#define MSG_MAXPAYLOAD 1024
#define MSG_MAXNAME 128

#define MSG_CLASS_SYS "sys"
#define MSG_CLASS_DEV "dev"

#define MSG_STOP     "sys:stop"

#define MSG_MOUSE_MOVE         "mouse"

#define MSG_KEYBOARD_KEYDOWN   "keyboard:keydown"
#define MSG_KEYBOARD_KEYUP     "keyboard:keyup"

typedef struct 
{
    uint id;
    uint reply;
    char name[MSG_MAXNAME];

    uint flags;
    
    uint to;
    uint from;

    void * payload;
    uint size;
} message_t;