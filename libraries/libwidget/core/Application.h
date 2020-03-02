#pragma once

#include <libsystem/utils/List.h>

#include <libwidget/core/Window.h>

void application_initialize(int argc, char **argv);

int application_run(void);

void application_dump(void);

void application_add_window(Window *window);

void application_remove_window(Window *window);

void application_blit_window(Window *window, Rectangle bound);
