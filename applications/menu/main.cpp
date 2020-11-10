#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/File.h>
#include <libsystem/json/Json.h>
#include <libsystem/process/Process.h>
#include <libwidget/Application.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>

struct MenuEntry
{
    String name;
    String comment;
    RefPtr<Icon> icon;
    String command;
};

MenuEntry menu_entry_create(const char *path)
{
    auto root = json::parse_file(path);

    MenuEntry entry{};

    if (root.is(json::OBJECT))
    {
        if (root.has("name"))
        {
            auto value = root.get("name");

            if (value.is(json::STRING))
            {
                entry.name = value.as_string();
            }
        }

        if (root.has("comment"))
        {
            auto value = root.get("comment");

            if (value.is(json::STRING))
            {
                entry.comment = value.as_string();
            }
        }

        if (root.has("icon"))
        {
            auto value = root.get("icon");

            if (value.is(json::STRING))
            {
                entry.icon = Icon::get(value.as_string());
            }
        }

        if (root.has("command"))
        {
            auto value = root.get("command");

            if (value.is(json::STRING))
            {
                entry.command = value.as_string();
            }
        }
    }

    return entry;
}

void load_menu(Vector<MenuEntry> &entries)
{
    Directory *directory = directory_open("/Applications", OPEN_READ);

    if (handle_has_error(directory))
    {
        directory_close(directory);
        return;
    }

    DirectoryEntry entry = {};
    while (directory_read(directory, &entry) > 0)
    {
        if (entry.stat.type == FILE_TYPE_DIRECTORY)
        {
            char path[PATH_LENGTH];
            snprintf(path, PATH_LENGTH, "/Applications/%s/manifest.json", entry.name);

            if (file_exist(path))
            {
                entries.push_back(menu_entry_create(path));
            }
        }
    }

    directory_close(directory);
}

void menu_create_list(Widget *parent, Vector<MenuEntry> &entries)
{
    Widget *list = new Container(parent);

    list->layout(VFLOW(4));
    list->attributes(LAYOUT_FILL);
    list->insets(Insets(4));

    entries.foreach ([&](MenuEntry &entry) {
        auto item = new Button(
            list,
            BUTTON_TEXT,
            entry.icon,
            entry.name);

        item->insets(Insets(8));

        item->on(Event::ACTION, [entry](auto) {
            process_run(entry.command.cstring(), nullptr);
            application_exit(0);
        });

        return Iteration::CONTINUE;
    });
}

int main(int argc, char **argv)
{
    application_initialize(argc, argv);

    Vector<MenuEntry> entries{};
    load_menu(entries);

    auto window = new Window(WINDOW_BORDERLESS | WINDOW_TRANSPARENT);

    window->title("Panel");
    window->position(Vec2i::zero());
    window->bound(Screen::bound().with_width(320));
    window->type(WINDOW_TYPE_POPOVER);
    window->opacity(0.95);

    window->root()->layout(HFLOW(0));

    auto containter = new Container(window->root());
    containter->layout(VFLOW(0));
    containter->attributes(LAYOUT_FILL);

    menu_create_list(containter, entries);

    auto bottom_container = new Panel(containter);

    bottom_container->layout(HFLOW(4));
    bottom_container->insets({6});

    new IconPanel(bottom_container, Icon::get("account"));

    auto user_name_label = new Label(bottom_container, "User");
    user_name_label->attributes(LAYOUT_FILL);

    auto folder_button = new Button(bottom_container, BUTTON_TEXT, Icon::get("folder"));

    folder_button->on(EventType::ACTION, [&](auto) {
        process_run("file-manager", nullptr);
    });

    auto setting_button = new Button(bottom_container, BUTTON_TEXT, Icon::get("cog"));

    setting_button->on(EventType::ACTION, [&](auto) {
        process_run("settings", nullptr);
    });

    new Button(bottom_container, BUTTON_TEXT, Icon::get("power-standby"));

    new Separator(window->root());

    window->show();

    return application_run();
}
