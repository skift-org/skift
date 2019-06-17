#pragma once

#include <skift/drawing.h>

#include "compositor/client.h"

typedef struct
{
    char tite[64];
    rectangle_t bound;

    hideo_client_t *client;
} hideo_window_t;

hideo_window_t* hideo_window(hideo_client_t* client, rectangle_t bound);

void hideo_window_delete(hideo_window_t* this);

void hideo_window_handle_request(hideo_window_t* this, message_t* request);
