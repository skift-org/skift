#include "mixer/Client.h"
#include "mixer/MixerProtocol.h"
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/system/Memory.h>
#include <libsystem/utils/Hexdump.h>
#include <libsystem/utils/List.h>

void client_request_callback(Client *client, Connection *connection, PollEvent events)
{
    assert(events & POLL_READ);

    MixerMessage message = {};
    size_t message_size = connection_receive(connection, &message, sizeof(MixerMessage));

    if (handle_has_error(connection))
    {
        logger_error("Client handle has error: %s!", handle_error_string(connection));

        client->disconnected = true;
        // client_destroy_disconnected();
        return;
    }

    if (message_size != sizeof(MixerMessage))
    {
        logger_error("Got a message with an invalid size from client %u != %u!", sizeof(MixerMessage), message_size);
        hexdump(&message, message_size);

        client->disconnected = true;
        // client_destroy_disconnected();

        return;
    }

    if (message.type == MIXER_MESSAGE_AUDIODATA)
    {
        if (client->mixed == 0)
        {
            client->mixed = 1;
            for (int i = 0; i < AUDIO_DATA_MESSAGE_SIZE; i++)
            {
                Client::mixed_buffer[i] = Client::mixed_buffer[i] + (message.audiodata.audiodata[i] / 1.5);
            }
        }
        else
        {
            client->signals_waiting += 1;
        }
    }
    else if (message.type == MIXER_MESSAGE_DISCONNECT)
    {
        client->disconnected = true;
        // client_destroy_disconnected();
    }
}

void Client::receive_message()
{
    MixerMessage message = {};
    size_t message_size = connection_receive(connection, &message, sizeof(MixerMessage));

    if (handle_has_error(connection))
    {
        logger_error("Client handle has error: %s!", handle_error_string(connection));

        disconnected = true;
        // client_destroy_disconnected();
        return;
    }

    if (message_size != sizeof(MixerMessage))
    {
        logger_error("Got a message with an invalid size from client %u != %u!", sizeof(MixerMessage), message_size);
        hexdump(&message, message_size);

        disconnected = true;
        // client_destroy_disconnected();

        return;
    }

    if (message.type == MIXER_MESSAGE_AUDIODATA)
    {
        if (mixed == 0)
        {
            mixed = 1;
            for (int i = 0; i < AUDIO_DATA_MESSAGE_SIZE; i++)
            {
                Client::mixed_buffer[i] = Client::mixed_buffer[i] + (message.audiodata.audiodata[i] / 1.5);
            }
        }
        else
        {
            signals_waiting += 1;
        }
    }
    else if (message.type == MIXER_MESSAGE_DISCONNECT)
    {
        disconnected = true;
        // client_destroy_disconnected();
    }
}

Client::Client(Connection *connection)
{

    this->connection = connection;
    this->notifier = notifier_create(
        this,
        HANDLE(connection),
        POLL_READ,
        (NotifierCallback)client_request_callback);
    logger_info("Client %08x connected", this);
    connected_clients++;
    MixerMessage a = MixerMessage();
    a.type = MIXER_MESSAGE_GREETINGS;
    this->send_message(a);
}

Client::~Client()
{
    logger_info("Disconnecting client %08x", this);
    connected_clients--;
    notifier_destroy(notifier);
    connection_close(connection);
}

// void client_broadcast(MixerMessage message)
// {
//     // list_foreach(Client, client, _connected_client)
//     // {
//     //     client->send_message(message);
//     // }
// }

Result Client::send_message(MixerMessage message)
{
    if (disconnected)
    {
        return ERR_STREAM_CLOSED;
    }

    connection_send(connection, &message, sizeof(MixerMessage));

    if (handle_has_error(connection))
    {
        logger_error("Failed to send message to %08x: %s", this, handle_error_string(connection));
        disconnected = true;
        return handle_get_error(connection);
    }

    return SUCCESS;
}

// Iteration client_destroy_if_disconnected(void *target, Client *client)
// {
//     __unused(target);

//     if (client->disconnected)
//     {
//         delete client;
//     }

//     return Iteration::CONTINUE;
// }

// void client_destroy_disconnected(Vector<Client *> &client_list)
// {
//     client_list.foreach (client_destroy_if_disconnected);
// }
