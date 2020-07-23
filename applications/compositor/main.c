#include <abi/Paths.h>

#include <libsystem/Assert.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Socket.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>
#include <libsystem/system/Logger.h>
#include <libsystem/unicode/UTF8Decoder.h>

#include "compositor/Client.h"
#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static EventType key_motion_to_event_type(KeyMotion motion)
{
    if (motion == KEY_MOTION_DOWN)
    {
        return EVENT_KEYBOARD_KEY_PRESS;
    }

    if (motion == KEY_MOTION_UP)
    {
        return EVENT_KEYBOARD_KEY_RELEASE;
    }

    if (motion == KEY_MOTION_TYPED)
    {
        return EVENT_KEYBOARD_KEY_TYPED;
    }

    ASSERT_NOT_REACHED();
}

void keyboard_callback(void *target, Stream *keyboard_stream, SelectEvent events)
{
    __unused(target);
    __unused(events);

    KeyboardPacket packet;
    size_t size = stream_read(keyboard_stream, &packet, sizeof(KeyboardPacket));

    if (size == sizeof(KeyboardPacket))
    {
        Window *window = manager_focus_window();

        if (window != NULL)
        {
            Event event = {
                .type = key_motion_to_event_type(packet.motion),
                .keyboard = {
                    .key = packet.key,
                    .codepoint = packet.codepoint,
                },
            };

            window_send_event(window, event);
        }
    }
    else
    {
        logger_warn("Invalid keyboard packet with size=%d !", size);
    }

    client_destroy_disconnected();
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
        logger_warn("Invalid mouse packet with size=%d !", size);
    }

    client_destroy_disconnected();
}

void accept_callback(void *target, Socket *socket, SelectEvent events)
{
    __unused(target);
    __unused(events);

    Connection *incoming_connection = socket_accept(socket);

    client_create(incoming_connection);

    client_destroy_disconnected();
}

void render_callback(void *target)
{
    __unused(target);

    renderer_repaint_dirty();

    client_destroy_disconnected();
}

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    eventloop_initialize();

    Stream *keyboard_stream = stream_open(KEYBOARD_EVENT_DEVICE_PATH, OPEN_READ);
    Stream *mouse_stream = stream_open(MOUSE_DEVICE_PATH, OPEN_READ);
    Socket *socket = socket_open("/Session/compositor.ipc", OPEN_CREATE);

    notifier_create(NULL, HANDLE(keyboard_stream), SELECT_READ, (NotifierCallback)keyboard_callback);
    notifier_create(NULL, HANDLE(mouse_stream), SELECT_READ, (NotifierCallback)mouse_callback);
    notifier_create(NULL, HANDLE(socket), SELECT_ACCEPT, (NotifierCallback)accept_callback);

    Timer *repaint_timer = timer_create(NULL, 1000 / 60, render_callback);
    timer_start(repaint_timer);

    manager_initialize();
    cursor_initialize();
    renderer_initialize();

    process_run("panel", NULL);

    return eventloop_run();
}
