#include <assert.h>

#include <libasync/Loop.h>
#include <libio/Format.h>
#include <libio/Socket.h>
#include <libio/Streams.h>
#include <libsystem/process/Process.h>
#include <libwidget/Application.h>
#include <libwidget/Screen.h>

namespace Widget
{

/* --- Context -------------------------------------------------------------- */

static Application *_instance = nullptr;

Application &Application::the()
{
    if (_instance == nullptr)
    {
        _instance = new Application();
    }

    return *_instance;
}

Application::Application()
{
    _instance = this;

    _setting_theme = own<Settings::Setting>(
        "appearance:widgets.theme",
        [this](const Json::Value &value) {
            auto new_theme = value.as_string();

            theme_load(IO::format("/Files/Themes/{}.json", value.as_string()));

            for (size_t i = 0; i < _windows.count(); i++)
            {
                _windows[i]->should_repaint(_windows[i]->bound());
            }
        });

    _setting_wireframe = own<Settings::Setting>(
        "appearance:widgets.wireframe",
        [this](const Json::Value &value) {
            _wireframe = value.as_bool();
            for (size_t i = 0; i < _windows.count(); i++)
            {
                _windows[i]->should_repaint(_windows[i]->bound());
            }
        });

    _connection = IO::Socket::connect("/Session/compositor.ipc").unwrap();

    _connection_notifier = own<Async::Notifier>(_connection, POLL_READ, [this]() {
        CompositorMessage message = {};
        auto read_result = _connection.read(&message, sizeof(CompositorMessage));

        if (!read_result.success())
        {
            IO::logln("Connection to the compositor closed {}!", read_result.description());
            this->exit(PROCESS_FAILURE);
        }

        size_t message_size = read_result.unwrap();

        if (message_size != sizeof(CompositorMessage))
        {
            IO::logln("Got a message with an invalid size from compositor {} != {}!", sizeof(CompositorMessage), message_size);
            this->exit(PROCESS_FAILURE);
        }

        do_message(message);
    });

    auto result_or_greetings = wait_for_message(COMPOSITOR_MESSAGE_GREETINGS);

    if (result_or_greetings.success())
    {
        auto greetings = result_or_greetings.unwrap();

        Screen::bound(greetings.greetings.screen_bound);
    }
}

Application::~Application() {}

/* --- IPC ------------------------------------------------------------------ */

void Application::send_message(CompositorMessage message)
{
    _connection.write(&message, sizeof(message));
}

void Application::do_message(const CompositorMessage &message)
{
    if (message.type == COMPOSITOR_MESSAGE_EVENT_WINDOW)
    {
        Window *window = get_window(message.event_window.id);

        if (window)
        {
            Event copy = message.event_window.event;
            window->dispatch_event(&copy);
        }
    }
    else if (message.type == COMPOSITOR_MESSAGE_CHANGED_RESOLUTION)
    {
        Screen::bound(message.changed_resolution.resolution);

        for (size_t i = 0; i < _windows.count(); i++)
        {
            Event e = {
                .type = Event::DISPLAY_SIZE_CHANGED,
                .accepted = false,
                .mouse = {},
                .keyboard = {},
            };

            _windows[i]->dispatch_event(&e);
        }
    }
    else
    {
        IO::logln("Got an invalid message from compositor ({})!", message.type);
        Application::exit(PROCESS_FAILURE);
    }
}

ResultOr<CompositorMessage> Application::wait_for_message(CompositorMessageType expected_message)
{
    Vector<CompositorMessage> pendings;

    CompositorMessage message{};
    TRY(_connection.read(&message, sizeof(message)));

    while (message.type != expected_message)
    {
        pendings.push_back(std::move(message));
        auto result = _connection.read(&message, sizeof(CompositorMessage));

        if (!result.success())
        {
            pendings.foreach([&](auto &message) {
                do_message(message);
                return Iteration::CONTINUE;
            });

            return result.result();
        }
    }

    pendings.foreach([&](auto &message) {
        do_message(message);
        return Iteration::CONTINUE;
    });

    return message;
}

void Application::wait_for_ack()
{
    wait_for_message(COMPOSITOR_MESSAGE_ACK);
}

void Application::show_window(Window *window)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_CREATE_WINDOW,
        .create_window = {
            .id = window->handle(),
            .flags = window->flags(),
            .type = window->type(),
            .frontbuffer = window->frontbuffer_handle(),
            .frontbuffer_size = window->frontbuffer_size(),
            .backbuffer = window->backbuffer_handle(),
            .backbuffer_size = window->backbuffer_size(),
            .bound = window->bound_on_screen(),
        },
    };

    send_message(message);
}

void Application::hide_window(Window *window)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_DESTROY_WINDOW,
        .destroy_window = {
            .id = window->handle(),
        },
    };

    send_message(message);
    exit_if_all_windows_are_closed();
}

void Application::flip_window(Window *window, Math::Recti dirty)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_FLIP_WINDOW,
        .flip_window = {
            .id = window->handle(),
            .frontbuffer = window->frontbuffer_handle(),
            .frontbuffer_size = window->frontbuffer_size(),
            .backbuffer = window->backbuffer_handle(),
            .backbuffer_size = window->backbuffer_size(),
            .dirty = dirty,
            .bound = window->bound_on_screen(),
        },
    };

    send_message(message);
    wait_for_ack();
}

void Application::move_window(Window *window, Math::Vec2i position)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_MOVE_WINDOW,
        .move_window = {
            .id = window->handle(),
            .position = position,
        },
    };

    send_message(message);
}

void Application::window_change_cursor(Window *window, CursorState state)
{
    assert(_windows.contains(window));

    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_CURSOR_WINDOW,
        .cursor_window = {
            .id = window->handle(),
            .state = state,
        },
    };

    send_message(message);
}

Math::Vec2i Application::mouse_position()
{
    CompositorMessage message = {
        .type = COMPOSITOR_MESSAGE_GET_MOUSE_POSITION,
        .mouse_position = {},
    };

    send_message(message);

    auto result_or_mouse_position = wait_for_message(COMPOSITOR_MESSAGE_MOUSE_POSITION);

    if (result_or_mouse_position.success())
    {
        auto mouse_position = result_or_mouse_position.unwrap();

        return mouse_position.mouse_position.position;
    }
    else
    {
        return Math::Vec2i::zero();
    }
}

void Application::goodbye()
{
    CompositorMessage m = {
        .type = COMPOSITOR_MESSAGE_GOODBYE,
        .mouse_position = {},
    };

    send_message(m);

    wait_for_ack();
}

/* --- Windows -------------------------------------------------------------- */

void Application::hide_all_windows()
{
    for (size_t i = 0; i < _windows.count(); i++)
    {
        _windows[i]->hide();
    }
}

void Application::exit_if_all_windows_are_closed()
{
    if (_exiting)
    {
        return;
    }

    bool should_close = true;

    for (size_t i = 0; i < _windows.count(); i++)
    {
        if (_windows[i]->visible())
        {
            should_close = false;
            break;
        }
    }

    if (should_close)
    {
        exit(PROCESS_SUCCESS);
    }
}

void Application::add_window(Window *window)
{
    _windows.push_back(window);
}

void Application::remove_window(Window *window)
{
    _windows.push_back(window);
    exit_if_all_windows_are_closed();
}

Window *Application::get_window(int id)
{
    for (size_t i = 0; i < _windows.count(); i++)
    {
        if (_windows[i]->handle() == id)
        {
            return _windows[i];
        }
    }

    return nullptr;
}

/* --- Application ---------------------------------------------------------- */

int Application::run()
{
    Assert::falsity(_exiting);

    auto window = build();

    if (window != nullptr)
    {
        if (window->flags() & WINDOW_FULLSCREEN)
        {
            window->bound(Screen::bound());
        }
        else
        {
            window->resize_to_content();
        }

        window->show();
    }

    return loop().run();
}

int Application::run_nested()
{
    Assert::falsity(_exiting);
    return loop().run_nested();
}

int Application::pump()
{
    Assert::falsity(_exiting);
    loop().pump(true);
    return 0;
}

void Application::exit(int exit_value)
{
    if (_exiting)
    {
        return;
    }

    _exiting = true;

    hide_all_windows();

    goodbye();

    _connection_notifier = nullptr;
    _connection.close();

    loop().exit(exit_value);
}

void Application::exit_nested(int exit_value)
{
    assert(!_exiting);
    loop().exit_nested(exit_value);
}

} // namespace Widget
