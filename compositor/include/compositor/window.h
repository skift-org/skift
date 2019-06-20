#pragma once

#include <skift/shape.h>
#include <skift/painter.h>

#include "compositor/client.h"
#include "compositor/server.h"

typedef struct
{
    int wid;
    char tite[64];
    rectangle_t bound;

    hideo_client_t *client;
} hideo_window_t;

hideo_window_t* hideo_window(hideo_server_t* server, hideo_client_t* client, rectangle_t bound);

void hideo_window_delete(hideo_window_t* this);

void hideo_window_handle_request(hideo_window_t* this, message_t* request);

void window_paint_decoration(painter_t *paint, rectangle_t bound, const char* title);
