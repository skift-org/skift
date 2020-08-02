#pragma once

#include <libgraphic/Icon.h>

struct Menu;

Menu *menu_create(Menu *parent, RefPtr<Icon> icon, const char *text);

void menu_destroy(Menu *menu);

void menu_show(Menu *menu);
