#pragma once

#include <libipc/Peer.h>
#include <libjson/Json.h>

#include <libsettings/Path.h>

namespace Settings
{

struct Message
{
    enum Type : uint8_t
    {
        CLIENT_READ,
        CLIENT_WRITE,
        CLIENT_WATCH,
        CLIENT_UNWATCH,

        SERVER_ACK,
        SERVER_VALUE,
        SERVER_NOTIFY,
    };

    Type type;
    Optional<Path> path;
    Optional<Json::Value> payload;
};

struct Protocol
{
    using Message = Settings::Message;

    static Result encode_message(IO::Connection &connection, const Message &message);

    static ResultOr<Message> decode_message(IO::Connection &connection);
};

} // namespace Settings
