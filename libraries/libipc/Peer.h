#pragma once

#include <libsystem/eventloop/Notifier.h>
#include <libsystem/io/Connection.h>
#include <libsystem/io/Socket.h>

#include <libutils/Callback.h>

namespace ipc
{

template <typename Protocol>
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
            auto result_or_message = Protocol::decode_message(_connection);

            if (result_or_message)
            {
                on_message(*result_or_message);
            }
            else
            {
                close();
            }
        });
    }

    virtual ~Peer()
    {
        close();
    }

    Result send(const Protocol::Message &message)
    {
        if (!_connection)
        {
            return ERR_STREAM_CLOSED;
        }

        auto result = Protocol::encode_message(_connection, message);

        if (result != SUCCESS)
        {
            close();
        }

        return result;
    }

    ResultOr<Protocol::Message> receive()
    {
        if (!_connection)
        {
            return ERR_STREAM_CLOSED;
        }

        auto result_or_message = Protocol::decode_message(_connection);

        if (!result_or_message.success())
        {
            close();
        }

        return result_or_message;
    }

    ResultOr<Protocol::Message> request(Protocol::Message message, Protocol::Type expected)
    {
        auto send_result = send(message);

        if (send_result != SUCCESS)
        {
            return send_result;
        }

        auto result_or_message = receive();

        while (result_or_message && (*result_or_message).type != expected)
        {
            on_message(*result_or_message);

            auto result_or_message = receive();
        }

        return result_or_message;
    }

    void close()
    {
        if (_connection)
        {
            on_disconnect();

            _notifier = nullptr;

            connection_close(_connection);
            _connection = nullptr;
        }
    }

    virtual void on_message(const Protocol::Message &) {}
    virtual void on_disconnect() {}
};

} // namespace ipc
