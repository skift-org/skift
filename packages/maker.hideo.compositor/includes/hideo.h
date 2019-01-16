#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/list.h>
#include <skift/drawing.h>

#define RESIZE_AREA    8
#define SNAP_AREA     32

#define WIN_TITLE_SIZE 32
#define WIN_HEADER_HEIGHT 32
#define WIN_RESIZE_AREA 8

typedef struct
{
    int x;
    int y;

    char title[WIN_TITLE_SIZE];

    uint width;
    uint height;
} hideo_window_t;

typedef enum
{
    BTN_PRESSED,
    BTN_RELEASED,
    BTN_UP,
    BTN_DOWN,
} hideo_button_state_t;

typedef struct 
{
    int process;
    list_t* windows;
} hideo_client_t;

typedef enum 
{
    HIDEO_RESIZE_WINDOW,
    HIDEO_MOVE_WINDOW
} hideo_state_t;

typedef struct 
{
    int x;
    int y;

    bitmap_t* oldscreen;
} hideo_mouse_t;

typedef struct 
{
    uint width;
    uint height;

    bool running;

    hideo_window_t *focus;

    list_t *windows;
    list_t *clients;

    bitmap_t * screen;
    hideo_mouse_t mouse;
} hideo_context_t;