#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/messaging.h>
#include <libsystem/list.h>

typedef struct 
{
    list_t* list;
    bool loop_continue;
} eventloop_t;

typedef void (*eventloop_message_handler_t)(eventloop_t* eventloop, message_t* message, void* payload);

typedef struct 
{
    const char* message_type;
    eventloop_message_handler_t handler;
} eventloop_handler_t;

eventloop_t* eventloop();

void eventloop_register_handler(eventloop_t* this, const char* message_type, eventloop_message_handler_t handler);

void eventloop_pump(eventloop_t* this, bool blocking);

void eventloop_subscribe(eventloop_t* this, const char* channel);

void eventloop_run(eventloop_t* this);
