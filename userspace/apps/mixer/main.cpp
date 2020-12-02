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

#include "Client.h"

Vector<Client *> client_list;
Stream *device;

int Client::Client::connected_clients = 0;
char Client::Client::mixed_buffer[AUDIO_DATA_MESSAGE_SIZE]{};

void client_destroy_disconnected()
{
    for (size_t i = 0; i < client_list.count(); i++)
    {
        if (client_list[i]->disconnected == true)
        {
            client_list.remove_index(i);
            i--;
        }
    }
}

void accept_callback(void *target, Socket *socket, PollEvent events)
{
    __unused(target);
    __unused(events);

    Connection *incoming_connection = socket_accept(socket);

    client_list.push_back(new Client(incoming_connection));

    client_destroy_disconnected();
}

void send_mixed_data()
{
    char buffer[AUDIO_DATA_MESSAGE_SIZE];
    // check pending messages
    for (size_t i = 0; i < client_list.count(); i++)
    {
        if (client_list[i]->mixed == 0 && client_list[i]->signals_waiting > 0)
        {
            // consume 1 pending signal
            client_list[i]->receive_message();

            client_list[i]->signals_waiting--;
        }

        // for all set mixed to zero
        client_list[i]->mixed = 0;
    }

    // finally send mixed buffer to audio device
    // copy to intermediate buffer and send message
    for (int i = 0; i < AUDIO_DATA_MESSAGE_SIZE; i++)
    {
        buffer[i] = Client::mixed_buffer[i];
    }
    stream_write(device, buffer, AUDIO_DATA_MESSAGE_SIZE);

    // set static buffer to zero
    for (int i = 0; i < AUDIO_DATA_MESSAGE_SIZE; i++)
    {
        Client::mixed_buffer[i] = 0;
    }
}

bool acquire_lock()
{
    Stream *socket_stream = stream_open("/Session/audio-mixer.lock", OPEN_READ);
    if (!handle_has_error(socket_stream))
    {
        stream_close(socket_stream);
        return false;
    }
    else
    {
        Stream *lock_stream = stream_open("/Session/audio-mixer.lock", OPEN_READ | OPEN_CREATE);
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
        stream_format(err_stream, "Audio mixer is already running.\n");
        return PROCESS_FAILURE;
    }

    for (size_t i = 0; i > AUDIO_DATA_MESSAGE_SIZE; i++)
    {
        Client::mixed_buffer[i] = 0;
    }

    eventloop_initialize();
    // create socket
    Socket *socket = socket_open("/Session/audio-mixer.ipc", OPEN_CREATE);
    device = stream_open("/Devices/sound", OPEN_WRITE | OPEN_CREATE);
    // add notifier callback to accept connection

    notifier_create(nullptr, HANDLE(socket), POLL_ACCEPT, (NotifierCallback)accept_callback);

    auto buffer_fill_timer = own<Timer>(160, []() {
        // before sending signal check connections with signal wait and take one message mix into buffer
        send_mixed_data();
        client_destroy_disconnected();
    });

    buffer_fill_timer->start();

    return eventloop_run();
}
