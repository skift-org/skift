#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/list.h>
#include <skift/messaging.h>

#include "compositor/server.h"

typedef struct
{
    int pid;
    list_t *windows;
} hideo_client_t;

hideo_client_t *hideo_client(int pid);

void hideo_client_delete(hideo_client_t *this);

void hideo_client_handle_request(hideo_client_t *this, message_t *request);

void hideo_client_respond(hideo_client_t *client, const char *label, void *payload, int payload_size);

void hideo_client_notify(hideo_client_t *client, const char *label, void *payload, int payload_size);