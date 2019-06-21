#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

#include <hjert/syscalls.h>
#include <hjert/message.h>

// Send an event message to a process.
int messaging_send(message_t* event);

// Send an event message to all subscriber of a channel.
int messaging_broadcast(const char *channel, message_t* event);

// Send an request message to a process.
int messaging_request(message_t* request, message_t* result, int timeout);

// Receive a message.
int messaging_receive(message_t* message, bool wait);

// Respond to a request.
int messaging_respond(message_t* request, message_t* result);

// Subscribe to a channel.
int messaging_subscribe(const char* channel);

// Unsubscribe from a channel.
int messaging_unsubscribe(const char* channel);
