#include <libio/Copy.h>
#include <libio/MemoryWriter.h>
#include <libsettings/Protocol.h>

namespace Settings
{

struct MessageHeader
{
    Message::Type type;
    size_t path_length;
    size_t payload_length;
};

Result Protocol::encode_message(IO::Connection &connection, const Message &message)
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
        Json::prettify(pretty, *message.payload);
        payload_buffer = pretty.finalize();
    }

    MessageHeader header;
    header.type = message.type;
    header.path_length = path_buffer.length();
    header.payload_length = payload_buffer.length();

    TRY(connection.write(&header, sizeof(MessageHeader)));

    if (path_buffer.length())
    {
        TRY(connection.write(path_buffer.cstring(), path_buffer.length()));
    }

    if (payload_buffer.length())
    {
        TRY(connection.write(payload_buffer.cstring(), payload_buffer.length()));
    }

    return SUCCESS;
}

ResultOr<Message> Protocol::decode_message(IO::Connection &connection)
{
    MessageHeader header;

    TRY(connection.read(&header, sizeof(header)));

    Message message;
    message.type = header.type;

    if (header.path_length > 0)
    {
        IO::MemoryWriter memory;
        TRY(IO::copy(connection, memory, header.path_length));
        String str = memory.string();
        message.path = Path::parse(str);
    }

    if (header.payload_length > 0)
    {
        IO::MemoryWriter memory;
        TRY(IO::copy(connection, memory, header.payload_length));
        String str = memory.string();
        message.payload = Json::parse(str);
    }

    return message;
}

} // namespace Settings
