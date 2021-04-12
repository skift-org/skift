#pragma once

#include <libasync/Notifier.h>
#include <libio/Connection.h>
#include <libio/Socket.h>
#include <libutils/Callback.h>
#include <libutils/ResultOr.h>

namespace IPC
{

template <typename Protocol>
class Peer
{
private:
    IO::Connection _connection;
    OwnPtr<Async::Notifier> _notifier;

public:
    bool connected() { return !_connection.closed(); }

    Peer(IO::Connection connection) : _connection{connection}
    {
        _notifier = own<Async::Notifier>(_connection, POLL_READ, [this]() {
            auto result_or_message = Protocol::decode_message(_connection);

            if (result_or_message.success())
            {
                handle_message(result_or_message.unwrap());
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
        auto result = Protocol::encode_message(_connection, message);

        if (result != SUCCESS)
        {
            close();
        }

        return result;
    }

    ResultOr<typename Protocol::Message> receive()
    {
        auto result_or_message = Protocol::decode_message(_connection);

        if (!result_or_message.success())
        {
            close();
        }

        return result_or_message;
    }

    template <typename TPredicate>
    ResultOr<typename Protocol::Message> send_and_wait_for(Protocol::Message message, TPredicate predicate)
    {
        TRY(send(message));

        auto response = TRY(receive());

        while (!predicate(response))
        {
            handle_message(message);
            response = TRY(receive());
        }

        return response;
    }

    void close()
    {
        if (_connection.closed())
            return;

        handle_disconnect();

        _notifier = nullptr;
        _connection.close();
    }

    virtual void handle_message(const Protocol::Message &) {}
    virtual void handle_disconnect() {}
};

} // namespace IPC
