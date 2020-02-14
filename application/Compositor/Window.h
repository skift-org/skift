#pragma once

#include <libgraphic/shape.h>

typedef struct Window
{
    Rectangle bound;
} Window;

Window *window_create(Rectangle bound);

void window_destroy(Window *window);

Rectangle window_bound(Window *window);
