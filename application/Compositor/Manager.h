#pragma once

struct Window;

void manager_initialize(void);

void manager_register_window(struct Window *window);

void manager_unregister_window(struct Window *window);