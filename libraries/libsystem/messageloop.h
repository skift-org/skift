#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/messaging.h>
#include <libsystem/list.h>

typedef struct 
{
    List* message_handlers;
    bool loop_continue;
} messageloop_t;

typedef void (*messageloop_message_handler_t)(messageloop_t* messageloop, message_t* message, void* payload);

typedef struct 
{
    const char* message_type;
    messageloop_message_handler_t handler;
} messageloop_handler_t;

void messageloop_init(int argc, char** argv);

void messageloop_fini(void);

void messageloop_register_handler(const char* message_type, messageloop_message_handler_t handler);

void messageloop_pump(bool blocking);

void messageloop_subscribe( const char* channel);

void messageloop_run(void);
