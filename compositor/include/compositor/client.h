#pragma once

#include <skift/list.h>

typedef struct
{
    int pid;
    list_t *window;
} hideo_client_t;

hideo_client_t *hideo_client(int pid);

void hideo_client_delete(hideo_client_t *this);

void hideo_client_handle_request(hideo_client_t* this, message_t* request);