#include <libjson/Json.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/process/Process.h>
#include <libwidget/Application.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

struct MenuEntry
{
    char *name;
    char *comment;
    char *icon;
    char *command;
};

MenuEntry *menu_entry_create(const char *path)
{
    JsonValue *root = json_parse_file(path);

    MenuEntry *entry = nullptr;

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

List *load_menu()
{
    List *menu = list_create();

    Directory *directory = directory_open("/Applications", OPEN_READ);

    if (handle_has_error(directory))
    {
        directory_close(directory);
        return menu;
    }

    DirectoryEntry entry = {};
    while (directory_read(directory, &entry) > 0)
    {
        if (entry.stat.type == FILE_TYPE_DIRECTORY)
        {
            char path[PATH_LENGTH];
            snprintf(path, PATH_LENGTH, "/Applications/%s/manifest.json", entry.name);

            MenuEntry *entry = menu_entry_create(path);

            if (entry != nullptr)
            {
                list_pushback(menu, entry);
            }
        }
    }

    directory_close(directory);
    return menu;
}

void menu_item_click(MenuEntry *entry, Widget *sender, Event *event)
{
    __unused(event);

    process_run(entry->command, nullptr);

    window_hide(sender->window);
}

void menu_create_list(Widget *parent, List *menu)
{
    Widget *list = container_create(parent);
    list->layout = VFLOW(4);
    list->layout_attributes = LAYOUT_FILL;
    list->insets = Insets(4);

    list_foreach(MenuEntry, entry, menu)
    {
        Widget *item = button_create_with_icon_and_text(list, BUTTON_TEXT, Icon::get(entry->icon), entry->name);
        item->insets = Insets(8);

        widget_set_event_handler(item, EVENT_ACTION, [entry](auto) {
            process_run(entry->command, nullptr);
            application_exit(0);
        });
    }
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    List *menu = load_menu();

    Window *window = window_create(WINDOW_BORDERLESS | WINDOW_POP_OVER);

    window_set_title(window, "Panel");
    window_set_position(window, Vec2i::zero());
    window_set_size(window, Vec2i(320, screen_get_bound().height()));

    window_root(window)->layout = VFLOW(8);

    panel_create(window_root(window));
    menu_create_list(window_root(window), menu);

    window_show(window);

    return application_run();
}
