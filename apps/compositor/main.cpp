#include <abi/Paths.h>

#include <assert.h>
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

    EventLoop::initialize();

    Stream *keyboard_stream = stream_open(KEYBOARD_DEVICE_PATH, OPEN_READ);
    Stream *mouse_stream = stream_open(MOUSE_DEVICE_PATH, OPEN_READ);

    Socket *socket = socket_open("/Session/compositor.ipc", OPEN_CREATE);

    auto keyboard_notifier = own<Notifier>(HANDLE(keyboard_stream), POLL_READ, [&]() {
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
    });

    auto mouse_notifier = own<Notifier>(HANDLE(mouse_stream), POLL_READ, [&]() {
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
    });

    auto socker_notifier = own<Notifier>(HANDLE(socket), POLL_ACCEPT, [&]() {
        Connection *incoming_connection = socket_accept(socket);

        Client::connect(incoming_connection);

        client_destroy_disconnected();
    });

    auto repaint_timer = own<Timer>(1000 / 60, []() {
        renderer_repaint_dirty();
        client_destroy_disconnected();
    });

    repaint_timer->start();

    manager_initialize();
    cursor_initialize();
    renderer_initialize();

    process_run("panel", nullptr);

    if (strcmp(__CONFIG__, "develop") == 0)
    {
        process_run("terminal", nullptr);
    }
    else
    {
        process_run("onboarding", nullptr);
    }

    return EventLoop::run();
}
