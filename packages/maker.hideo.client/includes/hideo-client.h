#pragma once

/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* hideo-client.h: Hideo client API                                           */

#include <skift/generic.h>

typedef struct 
{
    
} hideo_window_t;

typedef struct 
{

} hideo_application_t;

hideo_application_t* hideo_connect();

hideo_window_t* hideo_window(hideo_application_t *a, uint w, uint h, uint flags);
void hideo_window_close(hideo_application_t *a);
