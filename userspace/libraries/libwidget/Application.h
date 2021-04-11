#pragma once

#include <libwidget/Window.h>

namespace Widget
{

namespace Application
{

/* --- Server --------------------------------------------------------------- */

void show_window(Window *window);

void hide_window(Window *window);

void flip_window(Window *window, Math::Recti bound);

void move_window(Window *window, Math::Vec2i position);

void window_change_cursor(Window *window, CursorState state);

Math::Vec2i mouse_position();

/* --- Client --------------------------------------------------------------- */

void add_window(Window *window);

void remove_window(Window *window);

Window *get_window(int id);

Result initialize(int argc, char **argv);

int run();

int run_nested();

int pump();

void exit(int exit_value);

void exit_nested(int exit_value);

bool show_wireframe();

} // namespace Application

} // namespace Widget
