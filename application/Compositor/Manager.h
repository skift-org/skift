#pragma once

#include <libsystem/list.h>

struct Window;

void manager_initialize(void);

List *manager_get_windows(void);

void manager_register_window(struct Window *window);

void manager_unregister_window(struct Window *window);