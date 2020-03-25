#pragma once

#include <libgraphic/Shape.h>
#include <libsystem/utils/List.h>

struct Client;
struct Window;

void manager_initialize(void);

List *manager_get_windows(void);

struct Window *manager_get_window(struct Client *client, int id);

struct Window *manager_get_window_at(Point position);

void manager_register_window(struct Window *window);

void manager_unregister_window(struct Window *window);