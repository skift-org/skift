#pragma once

#include <skift/list.h>

typedef struct
{
    list_t* clients;
} hideo_server_t;

hideo_server_t* hideo_server(void);

void hideo_server_delete(hideo_server_t* this);

void hideo_server_handle_request(hideo_server_t* this, message_t* request);
