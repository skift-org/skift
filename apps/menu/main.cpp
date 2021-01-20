#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/File.h>
#include <libsystem/json/Json.h>
#include <libsystem/process/Environment.h>
#include <libsystem/process/Process.h>
#include <libwidget/Application.h>
#include <libwidget/Screen.h>
#include <libwidget/Widgets.h>
#include <libwidget/widgets/TextField.h>

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
        root.with("name", [&](auto &value) {
            entry.name = value.as_string();
        });

        root.with("comment", [&](auto &value) {
            entry.comment = value.as_string();
        });

        root.with("icon", [&](auto &value) {
            entry.icon = Icon::get(value.as_string());
        });

        root.with("command", [&](auto &value) {
            entry.command = value.as_string();
        });
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

            File manifest_file{path};
            if (manifest_file.exist())
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
    list->flags(Widget::FILL);
    list->insets(Insetsi(4));

    entries.foreach ([&](MenuEntry &entry) {
        auto item = new Button(
            list,
            BUTTON_TEXT,
            entry.icon,
            entry.name);

        item->insets(Insetsi(8));

        item->on(Event::ACTION, [entry](auto) {
            process_run(entry.command.cstring(), nullptr);
            Application::exit(PROCESS_SUCCESS);
        });

        return Iteration::CONTINUE;
    });
}

int main(int argc, char **argv)
{
    Application::initialize(argc, argv);

    Vector<MenuEntry> entries{};
    load_menu(entries);

    auto window = new Window(WINDOW_BORDERLESS | WINDOW_ACRYLIC | WINDOW_AUTO_CLOSE);

    window->title("Panel");
    window->position(Vec2i::zero());
    window->bound(Screen::bound().with_width(320));
    window->type(WINDOW_TYPE_POPOVER);
    window->opacity(0.85);

    window->root()->layout(HFLOW(0));

    auto container = new Container(window->root());
    container->layout(VFLOW(0));
    container->flags(Widget::FILL);

    auto seach_container = new Container(container);
    seach_container->layout(VFLOW(0));
    seach_container->insets({8});

    auto panel = new Panel(seach_container);
    panel->layout(HFLOW(4));
    panel->insets({6});
    panel->border_radius(6);
    panel->color(THEME_MIDDLEGROUND, Colors::WHITE);
    panel->layout(HFLOW(4));

    auto icon = new IconPanel(panel, Icon::get("search"));
    icon->color(THEME_FOREGROUND, Colors::BLACK);

    auto field = new TextField(panel, TextModel::empty());
    field->flags(Widget::FILL);
    field->color(THEME_FOREGROUND, Colors::BLACK);
    field->focus();

    menu_create_list(container, entries);

    auto bottom_container = new Panel(container);

    bottom_container->layout(HFLOW(4));
    bottom_container->insets({6});

    new Button(bottom_container, BUTTON_TEXT, Icon::get("account"), environment().get("POSIX").get("LOGNAME").as_string());

    new Spacer(bottom_container);

    auto folder_button = new Button(bottom_container, BUTTON_TEXT, Icon::get("folder"));

    folder_button->on(EventType::ACTION, [&](auto) {
        process_run("file-manager", nullptr);
    });

    auto setting_button = new Button(bottom_container, BUTTON_TEXT, Icon::get("cog"));

    setting_button->on(EventType::ACTION, [&](auto) {
        process_run("settings", nullptr);
    });

    auto logout_button = new Button(bottom_container, BUTTON_TEXT, Icon::get("power-standby"));
    logout_button->on(EventType::ACTION, [&](auto) {
        process_run("logout", nullptr);
    });

    new Separator(window->root());

    window->on(Event::KEYBOARD_KEY_PRESS, [&](Event *event) {
        if (event->keyboard.key == KEYBOARD_KEY_ESC)
        {
            Application::exit(PROCESS_SUCCESS);
        }
    });

    window->show();

    return Application::run();
}
