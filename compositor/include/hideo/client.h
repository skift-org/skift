#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <hideo/common.h>

#define HIDEO_CLIENT_CLASS "client"

/* --- Client connection to the server -------------------------------------- */

typedef struct
{
    char name[128];
} hideo_client_hello_t;

// Say hello to the server
#define HIDEO_CLIENT_HELLO \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "hello")

// Free all ressource of the application on the server size (close all window)
#define HIDEO_CLIENT_GOODBYE \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "goodbye")

/* --- Client window message ------------------------------------------------ */

// Create a new window, the server should respond with a HIDEO_SERVER_RESULT 
// equal to the window id or id < 0 on error
#define HIDEO_CLIENT_WINDOW_CREATE \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_create")

// Free all the ressources use by the window on the server side
#define HIDEO_CLIENT_WINDOW_DESTROY \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_destroy")

// Make the window visible
#define HIDEO_CLIENT_WINDOW_SHOW \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_show")

// Make the window **not** visible
#define HIDEO_CLIENT_WINDOW_HIDE \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_hide")

// Set the title of the window
#define HIDEO_CLIENT_WINDOW_SET_TITLE \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_set_title")

// Set the icon of the window
#define HIDEO_CLIENT_WINDOW_SET_ICON \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_set_icon")

// Set the position of the window 
#define HIDEO_CLIENT_WINDOW_SET_POSITION \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_set_position")

// Set the size of the window
#define HIDEO_CLIENT_WINDOW_SET_SIZE \
    MSGLABEL(HIDEO_PROTOCOL, HIDEO_CLIENT_CLASS, "window_set_size")
