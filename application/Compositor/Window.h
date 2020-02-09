#pragma once

#include <libgraphic/shape.h>

typedef struct Window
{
    Rectangle bound;
} Window;

Window *window_create(void);

void window_destroy(Window *window);
