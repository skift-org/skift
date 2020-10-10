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
    auto root = json::parse_file(path);

    MenuEntry *entry = nullptr;

    if (json::is(root, json::OBJECT))
    {
        entry = __create(MenuEntry);

        if (json::object_has(root, "name"))
        {
            auto value = json::object_get(root, "name");

            if (json::is(value, json::STRING))
            {
                entry->name = strdup(json::string_value(value));
            }
        }

        if (json::object_has(root, "comment"))
        {
            auto value = json::object_get(root, "comment");

            if (json::is(value, json::STRING))
            {
                entry->comment = strdup(json::string_value(value));
            }
        }

        if (json::object_has(root, "icon"))
        {
            auto value = json::object_get(root, "icon");

            if (json::is(value, json::STRING))
            {
                entry->icon = strdup(json::string_value(value));
            }
        }

        if (json::object_has(root, "command"))
        {
            auto value = json::object_get(root, "command");

            if (json::is(value, json::STRING))
            {
                entry->command = strdup(json::string_value(value));
            }
        }
    }

    json::destroy(root);
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

void menu_create_list(Widget *parent, List *menu)
{
    Widget *list = new Container(parent);
    list->layout(VFLOW(4));
    list->attributes(LAYOUT_FILL);
    list->insets(Insets(4));

    list_foreach(MenuEntry, entry, menu)
    {
        auto item = new Button(list, BUTTON_TEXT, Icon::get(entry->icon), entry->name);
        item->insets(Insets(8));

        item->on(Event::ACTION, [entry](auto) {
            process_run(entry->command, nullptr);
            application_exit(0);
        });
    }
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    List *menu = load_menu();

    Window *window = new Window(WINDOW_BORDERLESS | WINDOW_TRANSPARENT);

    window->title("Panel");
    window->position(Vec2i::zero());
    window->bound(Screen::bound().with_width(320));
    window->type(WINDOW_TYPE_POPOVER);
    window->opacity(0.95);

    window->root()->layout(HFLOW(0));

    auto containter = new Container(window->root());
    containter->layout(VFLOW(0));
    containter->attributes(LAYOUT_FILL);

    menu_create_list(containter, menu);

    auto bottom_container = new Panel(containter);

    bottom_container->layout(HFLOW(4));
    bottom_container->insets({6});

    new IconPanel(bottom_container, Icon::get("account"));

    auto user_name_label = new Label(bottom_container, "User");
    user_name_label->attributes(LAYOUT_FILL);

    Button* foldet_button = new Button(bottom_container, BUTTON_TEXT, Icon::get("folder"));
    foldet_button->on(EventType::ACTION, [&](auto){
        process_run("file-manager", nullptr);
    });
    
    Button* setting_button = new Button(bottom_container, BUTTON_TEXT, Icon::get("cog"));
    setting_button->on(EventType::ACTION, [&](auto){
        process_run("settings", nullptr);
    });
    new Button(bottom_container, BUTTON_TEXT, Icon::get("power-standby"));

    new Separator(window->root());

    window->show();

    return application_run();
}
