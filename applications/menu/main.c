#include <libjson/Json.h>
#include <libsystem/CString.h>
#include <libsystem/Logger.h>
#include <libsystem/io/Directory.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Application.h>
#include <libwidget/Widgets.h>

typedef struct
{
    char *name;
    char *comment;
    char *icon;
    char *command;
} MenuEntry;

MenuEntry *menu_entry_create(const char *path)
{
    JsonValue *root = json_parse_file(path);

    MenuEntry *entry = NULL;

    if (json_is(root, JSON_OBJECT))
    {
        entry = __create(MenuEntry);

        if (json_object_has(root, "name"))
        {
            JsonValue *value = json_object_get(root, "name");

            if (json_is(value, JSON_STRING))
            {
                entry->name = strdup(json_string_value(value));
            }
        }

        if (json_object_has(root, "comment"))
        {
            JsonValue *value = json_object_get(root, "comment");

            if (json_is(value, JSON_STRING))
            {
                entry->comment = strdup(json_string_value(value));
            }
        }

        if (json_object_has(root, "icon"))
        {
            JsonValue *value = json_object_get(root, "icon");

            if (json_is(value, JSON_STRING))
            {
                entry->icon = strdup(json_string_value(value));
            }
        }

        if (json_object_has(root, "command"))
        {
            JsonValue *value = json_object_get(root, "command");

            if (json_is(value, JSON_STRING))
            {
                entry->command = strdup(json_string_value(value));
            }
        }
    }

    json_destroy(root);
    return entry;
}

void menu_entry_destroy(MenuEntry *entry)
{
    free(entry->name);
    free(entry->comment);
    free(entry->icon);
    free(entry->command);
    free(entry);
}

List *load_menu(void)
{
    List *menu = list_create();

    Directory *directory = directory_open("/cfg/menu", OPEN_READ);

    DirectoryEntry entry = {};
    while (directory_read(directory, &entry) > 0)
    {
        if (entry.stat.type == FILE_TYPE_REGULAR)
        {
            char path[256];
            snprintf(path, 256, "/cfg/menu/%s", entry.name);

            MenuEntry *entry = menu_entry_create(path);

            if (entry != NULL)
            {
                list_pushback(menu, entry);
            }
        }
    }

    return menu;
}

void menu_item_click(MenuEntry *entry, Widget *sender, Event *event)
{
    __unused(event);

    Launchpad *launchpad = launchpad_create(entry->name, entry->command);
    launchpad_launch(launchpad, NULL);

    window_hide(sender->window);
}

void menu_create_list(Widget *parent, List *menu)
{
    Widget *list = container_create(parent);
    list->layout = VFLOW(4);
    list->layout_attributes = LAYOUT_FILL;
    list->insets = INSETS(4);

    list_foreach(MenuEntry, entry, menu)
    {
        Widget *item = button_create_with_icon_and_text(list, BUTTON_TEXT, icon_get(entry->icon), entry->name);
        item->insets = INSETS(8);
        widget_set_event_handler(item, EVENT_ACTION, EVENT_HANDLER(entry, (EventHandlerCallback)menu_item_click));
    }
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    List *menu = load_menu();

    Window *window = window_create(320, 768, WINDOW_BORDERLESS | WINDOW_POP_OVER);

    window_set_title(window, "Panel");

    window_root(window)->layout = VFLOW(8);

    panel_create(window_root(window));
    menu_create_list(window_root(window), menu);

    window_show(window);

    return application_run();
}
