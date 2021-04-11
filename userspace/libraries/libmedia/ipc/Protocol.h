#pragma once

#include <libsystem/Common.h>
#include <libsystem/io/Connection.h>

#include <libutils/Optional.h>
#include <libutils/ResultOr.h>
#include <libutils/Slice.h>

namespace media
{

struct Protocol
{
    enum class Type : uint8_t
    {
        INVALID,
        ACK,
        GREETINGS,
        AUDIODATA,
        DISCONNECT,
    };

    struct Message
    {
        Type type;
        Optional<Slice> data;
    };

    struct SerializedMessageHeader
    {
        Type type;
        size_t payload;
    };

    ResultOr<Message> decode_message(Connection *connection)
    {
        SerializedMessageHeader header;

        connection_receive(connection, &header, sizeof(SerializedMessageHeader));

        if (handle_has_error(connection))
        {
            return handle_get_error(connection);
        }

        if (!header.payload)
        {
            return Message{header.type};
        }

        auto storage = make<SliceStorage>(header.payload);
        connection_receive(connection, storage->start(), header.payload);

        if (handle_has_error(connection))
        {
            return handle_get_error(connection);
        }

        return Message{header.type, Slice{storage}};
    }

    Result encode_message(Connection *connection, const Message &message)
    {
        SerializedMessageHeader header = {message.type};

        if (message.data)
        {
            header.payload = message.data->size();
        }
    }
};

} // namespace media
