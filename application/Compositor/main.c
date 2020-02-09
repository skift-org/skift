#include <libsystem/eventloop/EventLoop.h>
#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

#include "Compositor/Cursor.h"
#include "Compositor/Manager.h"
#include "Compositor/Renderer.h"

void keyboard_callback(Notifier *notifier, Stream *keyboard_stream)
{
    __unused(notifier);
    __unused(keyboard_stream);

    logger_info("Keyboard event!");

    char buffer[521];
    stream_read(keyboard_stream, buffer, 521);
}

void mouse_callback(Notifier *notifier, Stream *mouse_stream)
{
    __unused(notifier);
    __unused(mouse_stream);

    MousePacket packet;
    size_t size = stream_read(mouse_stream, &packet, sizeof(MousePacket));

    if (size == sizeof(MousePacket))
    {
        //logger_info("MousePacket{offx=%d, offy=%d, scroll=%d, left=%d, right=%d, midlle=%d}", packet.offx, packet.offy, packet.scroll, packet.left, packet.right, packet.middle);

        cursor_handle_packet(packet);
        renderer_blit();
    }
    else
    {
        logger_warn("Invalid mouse packet!");
    }
}

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    eventloop_initilize();

    Stream *keyboard_stream = stream_open("/dev/keyboard", OPEN_READ);
    Notifier *keyboard_notifier = notifier_create(HANDLE(keyboard_stream), SELECT_READ);
    keyboard_notifier->on_ready_to_read = (NotifierHandler)keyboard_callback;

    Stream *mouse_stream = stream_open("/dev/mouse", OPEN_READ);
    Notifier *mouse_notifier = notifier_create(HANDLE(mouse_stream), SELECT_READ);
    mouse_notifier->on_ready_to_read = (NotifierHandler)mouse_callback;

    manager_initialize();
    cursor_initialize();
    renderer_initialize();

    return eventloop_run();
}
