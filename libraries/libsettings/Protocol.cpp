#include <libsettings/Protocol.h>

namespace Settings
{

struct MessageHeader
{
    Message::Type type;
    size_t path_length;
    size_t payload_length;
};

Result Protocol::encode_message(Connection *connection, const Message &message)
{
    String path_buffer = "";

    if (message.path)
    {
        Prettifier pretty;
        message.path->prettify(pretty);
        path_buffer = pretty.finalize();
    }

    String payload_buffer = "";

    if (message.payload)
    {
        Prettifier pretty;
        json::prettify(pretty, *message.payload);
        payload_buffer = pretty.finalize();
    }

    MessageHeader header;
    header.type = message.type;
    header.path_length = path_buffer.length();
    header.payload_length = payload_buffer.length();

    connection_send(connection, &header, sizeof(MessageHeader));

    if (handle_has_error(connection))
    {
        return handle_get_error(connection);
    }

    if (path_buffer.length())
    {
        connection_send(connection, path_buffer.cstring(), path_buffer.length());

        if (handle_has_error(connection))
        {
            return handle_get_error(connection);
        }
    }

    if (payload_buffer.length())
    {
        connection_send(connection, payload_buffer.cstring(), payload_buffer.length());

        if (handle_has_error(connection))
        {
            return handle_get_error(connection);
        }
    }

    return SUCCESS;
}

ResultOr<Message> Protocol::decode_message(Connection *connection)
{
    MessageHeader header;
    connection_receive(connection, &header, sizeof(header));

    if (handle_has_error(connection))
    {
        return handle_get_error(connection);
    }

    Message message;

    message.type = header.type;

    if (header.path_length > 0)
    {
        auto *buffer = new char[header.path_length];
        connection_receive(connection, buffer, header.path_length);

        if (handle_has_error(connection))
        {
            delete[] buffer;
            return handle_get_error(connection);
        }

        message.path = Path::parse(buffer, header.path_length);
        delete[] buffer;
    }

    if (header.payload_length > 0)
    {
        auto *buffer = new char[header.payload_length];
        connection_receive(connection, buffer, header.payload_length);

        if (handle_has_error(connection))
        {
            delete[] buffer;
            return handle_get_error(connection);
        }

        message.payload = json::parse(buffer, header.payload_length);
        delete[] buffer;
    }

    return message;
}

} // namespace Settings
