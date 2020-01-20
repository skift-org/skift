#pragma once

#include <libsystem/list.h>

#include <libwidget/core/Window.h>

void application_initialize(int argc, char **argv);

int application_run(void);

void application_dump(void);

void application_add_window(Window *window);

void application_remove_window(Window *window);
