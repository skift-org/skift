#include <abi/Paths.h>

#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/eventloop/Timer.h>
#include <libsystem/io/Socket.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>
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
        return Event::KEYBOARD_KEY_PRESS;
    }

    if (motion == KEY_MOTION_UP)
    {
        return Event::KEYBOARD_KEY_RELEASE;
    }

    if (motion == KEY_MOTION_TYPED)
    {
        return Event::KEYBOARD_KEY_TYPED;
    }

    ASSERT_NOT_REACHED();
}

void keyboard_callback(void *target, Stream *keyboard_stream, PollEvent events)
{
    __unused(target);
    __unused(events);

    KeyboardPacket packet;
    size_t size = stream_read(keyboard_stream, &packet, sizeof(KeyboardPacket));

    if (size == sizeof(KeyboardPacket))
    {
        Window *window = manager_focus_window();

        if (window)
        {
            Event event = {
                .type = key_motion_to_event_type(packet.motion),
                .accepted = false,
                .mouse = {},
                .keyboard = {
                    .key = packet.key,
                    .modifiers = packet.modifiers,
                    .codepoint = packet.codepoint,
                },
            };

            window->send_event(event);
        }
    }
    else
    {
        logger_warn("Invalid keyboard packet with size=%d !", size);
    }

    client_destroy_disconnected();
}

void mouse_callback(void *target, Stream *mouse_stream, PollEvent events)
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

void accept_callback(void *target, Socket *socket, PollEvent events)
{
    __unused(target);
    __unused(events);

    Connection *incoming_connection = socket_accept(socket);

    new Client(incoming_connection);

    client_destroy_disconnected();
}

void render_callback(void *target)
{
    __unused(target);

    renderer_repaint_dirty();

    client_destroy_disconnected();
}

bool acquire_lock()
{
    Stream *socket_stream = stream_open("/Session/compositor.lock", OPEN_READ);
    if (!handle_has_error(socket_stream))
    {
        stream_close(socket_stream);
        return false;
    }
    else
    {
        Stream *lock_stream = stream_open("/Session/compositor.lock", OPEN_READ | OPEN_CREATE);
        stream_close(lock_stream);
        return true;
    }
}

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    if (!acquire_lock())
    {
        stream_format(err_stream, "The compositor is already running.\n");
        return PROCESS_FAILURE;
    }

    eventloop_initialize();

    Stream *keyboard_stream = stream_open(KEYBOARD_DEVICE_PATH, OPEN_READ);
    Stream *mouse_stream = stream_open(MOUSE_DEVICE_PATH, OPEN_READ);

    Socket *socket = socket_open("/Session/compositor.ipc", OPEN_CREATE);

    notifier_create(nullptr, HANDLE(keyboard_stream), POLL_READ, (NotifierCallback)keyboard_callback);
    notifier_create(nullptr, HANDLE(mouse_stream), POLL_READ, (NotifierCallback)mouse_callback);
    notifier_create(nullptr, HANDLE(socket), POLL_ACCEPT, (NotifierCallback)accept_callback);

    auto repaint_timer = own<Timer>(1000 / 144, []() {
        renderer_repaint_dirty();
        client_destroy_disconnected();
    });

    repaint_timer->start();

    manager_initialize();
    cursor_initialize();
    renderer_initialize();

    process_run("panel", nullptr);
    process_run("terminal", nullptr);

    return eventloop_run();
}
