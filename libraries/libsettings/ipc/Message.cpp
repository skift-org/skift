#include <libsettings/ipc/Message.h>

namespace settings
{

struct SerializedMessage
{
    MessageType type;
    size_t path_length;
    size_t payload_length;
};

ResultOr<Message> Message::read_from(Connection *connection)
{
    SerializedMessage header;
    connection_receive(connection, &header, sizeof(header));

    if (handle_has_error(connection))
    {
        return handle_get_error(connection);
    }

    Message message;

    message.type = header.type;

    if (header.path_length > 0)
    {
        auto buffer = new char[header.path_length];
        connection_receive(connection, buffer, header.path_length);

        if (handle_has_error(connection))
        {
            delete buffer;
            return handle_get_error(connection);
        }

        message.path = Path::parse(buffer, header.path_length);
        delete buffer;
    }

    if (header.payload_length > 0)
    {
        auto buffer = new char[header.payload_length];
        connection_receive(connection, buffer, header.payload_length);

        if (handle_has_error(connection))
        {
            delete buffer;
            return handle_get_error(connection);
        }

        message.payload = json::parse(buffer, header.payload_length);
        delete buffer;
    }

    return message;
}

Result Message::write_to(Connection *connection) const
{
    String path_buffer = "";

    if (path)
    {
        Prettifier pretty;
        path->prettify(pretty);
        path_buffer = pretty.finalize();
    }

    String payload_buffer = "";

    if (payload)
    {
        Prettifier pretty;
        json::prettify(pretty, *payload);
        payload_buffer = pretty.finalize();
    }

    SerializedMessage header;
    header.type = type;
    header.path_length = path_buffer.length();
    header.payload_length = payload_buffer.length();

    connection_send(connection, &header, sizeof(SerializedMessage));

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

} // namespace settings
