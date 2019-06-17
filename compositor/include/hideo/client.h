#pragma once

#include <hideo/common.h>

/* --- Client connection to the server -------------------------------------- */

#define HIDEO_CLIENT_HELLO "hideo:client.hello"

typedef struct 
{
    char name[128];
} hideo_client_hello_t;

#define HIDEO_CLIENT_GOODBYE "hideo:client.goodbye"

/* --- Client window message ------------------------------------------------ */

#define HIDEO_CLIENT_WINDOW_CREATE "hideo:client.window_create"

typedef struct
{
    int x;
    int y;

    int width;
    int height;
} hideo_client_window_create_t;

#define HIDEO_CLIENT_WINDOW_DESTROY "hideo:client.window_create"
