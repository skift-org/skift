#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>

#include <libsettings/ipc/Message.h>

namespace settings
{

class Peer
{
private:
    Connection *_connection = nullptr;
    OwnPtr<Notifier> _notifier = nullptr;

public:
    bool connected()
    {
        return _connection != nullptr;
    }

    Peer(Connection *connection)
    {
        _connection = connection;
        _notifier = own<Notifier>(HANDLE(_connection), POLL_READ, [this]() {
            auto result_of_message = Message::read_from(_connection);

            if (!result_of_message.success())
            {
                close();
            }
            else
            {
                handle_message(result_of_message.value());
            }
        });
    }

    virtual ~Peer()
    {
        close();
    }

    Result send(Message &message)
    {
        if (!_connection)
        {
            return ERR_STREAM_CLOSED;
        }

        auto result = message.write_to(_connection);

        if (result != SUCCESS)
        {
            close();
        }

        return result;
    }

    virtual void handle_message(const Message &) {}

    ResultOr<Message> receive()
    {
        if (!_connection)
        {
            return ERR_STREAM_CLOSED;
        }

        auto result_or_message = Message::read_from(_connection);

        if (!result_or_message.success())
        {
            close();
        }

        return result_or_message;
    }

    ResultOr<Message> request(Message message, MessageType expected)
    {
        auto send_result = send(message);

        if (send_result != SUCCESS)
        {
            return send_result;
        }

        auto result_or_message = receive();

        while (result_or_message.success() && result_or_message.value().type != expected)
        {
            handle_message(result_or_message.value());

            auto result_or_message = receive();
        }

        return result_or_message;
    }

    void close()
    {
        if (_connection)
        {
            disconnected();

            _notifier = nullptr;

            connection_close(_connection);
            _connection = nullptr;
        }
    }

    virtual void disconnected() {}
};

} // namespace settings
