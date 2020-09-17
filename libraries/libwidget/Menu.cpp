#include <libsystem/core/CString.h>
#include <libwidget/Menu.h>
#include <libwidget/Widgets.h>
#include <libwidget/Window.h>

struct Menu
{
    RefPtr<Icon> icon;
    char *text;
    List *items;
};

Menu *menu_create(Menu *parent, RefPtr<Icon> icon, const char *text)
{
    Menu *menu = __create(Menu);

    menu->icon = icon;
    menu->text = strdup(text);
    menu->items = list_create();

    if (parent)
    {
        list_pushback(parent->items, menu);
    }

    return menu;
}

void menu_destroy(Menu *menu)
{
    list_destroy_with_callback(menu->items, (ListDestroyElementCallback)menu_destroy);
    free(menu->text);
    free(menu);
}

void menu_show(Menu *menu)
{
    Window *window = new Window(WINDOW_BORDERLESS);
    window->size(Vec2i(128, menu->items->count() * 32));

    window->root()->layout(VGRID(4));

    list_foreach(Menu, item, menu->items)
    {
        new Button(window->root(), BUTTON_TEXT, item->icon, item->text);
    }

    window->show();
}
