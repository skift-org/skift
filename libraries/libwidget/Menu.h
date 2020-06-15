#pragma once

#include <libgraphic/Icon.h>

typedef struct Menu Menu;

Menu *menu_create(Menu *parent, Icon *icon, const char *text);

void menu_destroy(Menu *menu);

void menu_show(Menu *menu);
