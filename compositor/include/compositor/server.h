#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/list.h>
#include <skift/messaging.h>

typedef struct
{
    list_t *clients;
    list_t *windows;
} hideo_server_t;

hideo_server_t *hideo_server(void);

void hideo_server_delete(hideo_server_t *this);

void hideo_server_handle_request(hideo_server_t *this, message_t *request);
