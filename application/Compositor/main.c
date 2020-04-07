#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Socket.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/process/Launchpad.h>

#include "Compositor/Client.h"
#include "Compositor/Cursor.h"
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"
#include "Compositor/Window.h"

void keyboard_callback(void *target, Stream *keyboard_stream, SelectEvent events)
{
    __unused(target);
    __unused(events);

    char c;
    stream_read(keyboard_stream, &c, sizeof(char));

    Window *window = manager_focus_window();

    if (window != NULL)
    {
        KeyboardEvent event = (KeyboardEvent){{EVENT_KEYBOARD_KEY_TYPED, false}, c};
        window_send_event(window, (Event *)&event, sizeof(KeyboardEvent));
    }
}

void mouse_callback(void *target, Stream *mouse_stream, SelectEvent events)
{
    __unused(target);
    __unused(events);

    MousePacket packet;
    size_t size = stream_read(mouse_stream, &packet, sizeof(MousePacket));

    if (size == sizeof(MousePacket))
    {
        cursor_handle_packet(packet);
    }
    else
    {
        logger_warn("Invalid mouse packet!");
    }
}

void accept_callback(void *target, Socket *socket, SelectEvent events)
{
    __unused(target);
    __unused(events);

    Connection *incoming_connection = socket_accept(socket);

    client_create(incoming_connection);
}

void render_callback(void *target)
{
    __unused(target);

    renderer_repaint_dirty();
}

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    eventloop_initialize();

    Stream *keyboard_stream = stream_open("/dev/keyboard", OPEN_READ);
    Stream *mouse_stream = stream_open("/dev/mouse", OPEN_READ);
    Socket *socket = socket_open("/srv/compositor.ipc", OPEN_CREATE);

    notifier_create(NULL, HANDLE(keyboard_stream), SELECT_READ, (NotifierCallback)keyboard_callback);
    notifier_create(NULL, HANDLE(mouse_stream), SELECT_READ, (NotifierCallback)mouse_callback);
    notifier_create(NULL, HANDLE(socket), SELECT_ACCEPT, (NotifierCallback)accept_callback);

    Timer *repaint_timer = timer_create(NULL, 1000 / 60, render_callback);
    timer_start(repaint_timer);

    manager_initialize();
    cursor_initialize();
    renderer_initialize();

    /***
    Launchpad *demoLines = launchpad_create("__demolines", "/bin/__demolines");
    launchpad_launch(demoLines, NULL);

    Launchpad *demo3D = launchpad_create("__democube", "/bin/__democube");
    launchpad_launch(demo3D, NULL);

    Launchpad *democolors = launchpad_create("__democolors", "/bin/__democolors");
    launchpad_launch(democolors, NULL);

    Launchpad *demogfx = launchpad_create("__demogfx", "/bin/__demogfx");
    launchpad_launch(demogfx, NULL);
    ***/

    Launchpad *terminal = launchpad_create("Terminal", "/bin/Terminal");
    launchpad_launch(terminal, NULL);

    Launchpad *widgets = launchpad_create("WidgetFactory", "/bin/WidgetFactory");
    launchpad_launch(widgets, NULL);

    return eventloop_run();
}
