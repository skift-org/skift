#include <stdio.h>
#include <skift/messaging.h>
#include <skift/drawing.h>

#include <hideo-protocol.h>

#include "exemple-form.h"

int program()
{
    hideo_create_window_t payload = 
    {
        .width = 300,
        .height = 300
    };
    
    sk_messaging_broadcast("hideo:compositor", "window:create", &payload, sizeof(hideo_create_window_t), 0);
    return 0;
}