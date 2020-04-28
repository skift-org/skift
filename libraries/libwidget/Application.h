#pragma once

#include <libsystem/utils/List.h>

#include <libwidget/Window.h>

Result application_initialize(int argc, char **argv);

int application_run(void);

void application_exit(int exit_value);

void application_dump(void);

void application_add_window(Window *window);

void application_remove_window(Window *window);

void application_show_window(Window *window);

void application_hide_window(Window *window);

void application_blit_window(Window *window, Rectangle bound);

void application_move_window(Window *window, Point position);

void application_resize_window(Window *window, Rectangle bound);

void application_window_change_cursor(Window *window, CursorState state);

Window *application_get_window_by_id(int id);
