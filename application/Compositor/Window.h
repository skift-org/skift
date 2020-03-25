#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/Shape.h>
#include <libwidget/core/Event.h>

struct Client;

typedef struct Window
{
    int id;
    struct Client *client;
    Rectangle bound;

    Bitmap *framebuffer;
} Window;

Window *window_create(int id, struct Client *client, Rectangle bound, Bitmap *framebuffer);

void window_destroy(Window *window);

Rectangle window_bound(Window *window);

void window_send_event(Window *window, Event *event, size_t size);