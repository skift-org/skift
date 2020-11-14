#pragma once

#include <libwidget/Window.h>

Result application_initialize(int argc, char **argv);

int application_run();

int application_run_nested();

int application_pump();

void application_exit(int exit_value);

void application_exit_nested(int exit_value);

bool application_is_debbuging_layout();

void application_add_window(Window *window);

void application_remove_window(Window *window);

Window *application_get_window(int id);

void application_show_window(Window *window);

void application_hide_window(Window *window);

void application_flip_window(Window *window, Rectangle bound);

void application_move_window(Window *window, Vec2i position);

void application_resize_window(Window *window, Rectangle bound);

void application_window_change_cursor(Window *window, CursorState state);

Vec2i application_get_mouse_position();
