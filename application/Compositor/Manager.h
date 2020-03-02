#pragma once

#include <libsystem/utils/List.h>

struct Client;
struct Window;

void manager_initialize(void);

List *manager_get_windows(void);

struct Window *manager_get_window(struct Client *client, int id);

void manager_register_window(struct Window *window);

void manager_unregister_window(struct Window *window);