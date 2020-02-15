#pragma once

#include <libgraphic/bitmap.h>
#include <libgraphic/shape.h>

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
