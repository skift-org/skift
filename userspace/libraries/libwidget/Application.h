#pragma once

#include <libasync/Notifier.h>
#include <libasync/Source.h>
#include <libio/Connection.h>
#include <libsettings/Setting.h>
#include <libwidget/Window.h>

#include "compositor/Protocol.h"

namespace Widget
{

class Application :
    public Async::Source,
    public RefCounted<Application>
{
private:
    Vector<Window *> _windows;
    IO::Connection _connection;
    OwnPtr<Async::Notifier> _connection_notifier;
    OwnPtr<Settings::Setting> _setting_theme;
    OwnPtr<Settings::Setting> _setting_wireframe;
    bool _wireframe = false;
    bool _exiting = false;

    void send_message(CompositorMessage message);
    void do_message(const CompositorMessage &message);
    ResultOr<CompositorMessage> wait_for_message(CompositorMessageType expected_message);
    void hide_all_windows();
    void uninitialized();
    void wait_for_ack();
    void exit_if_all_windows_are_closed();
    void goodbye();

public:
    static RefPtr<Application> the();

    Application();

    void show_window(Window *window);

    void hide_window(Window *window);

    void flip_window(Window *window, Math::Recti bound);

    void move_window(Window *window, Math::Vec2i position);

    void window_change_cursor(Window *window, CursorState state);

    Math::Vec2i mouse_position();

    void add_window(Window *window);

    void remove_window(Window *window);

    Window *get_window(int id);

    Result initialize(int argc, char **argv);

    int run();

    int run_nested();

    int pump();

    void exit(int exit_value);

    void exit_nested(int exit_value);

    bool show_wireframe() { return _wireframe; }
};

} // namespace Widget
