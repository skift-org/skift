#pragma once

#include <libwidget/Window.h>

namespace Application
{

Result initialize(int argc, char **argv);

int run();

int run_nested();

int pump();

void exit(int exit_value);

void exit_nested(int exit_value);

bool show_wireframe();

void add_window(Window *window);

void remove_window(Window *window);

Window *get_window(int id);

void show_window(Window *window);

void hide_window(Window *window);

void flip_window(Window *window, Recti bound);

void move_window(Window *window, Vec2i position);

void resize_window(Window *window, Recti bound);

void window_change_cursor(Window *window, CursorState state);

Vec2i mouse_position();

} // namespace Application
