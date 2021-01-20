#pragma once

#include <libsystem/Common.h>
#include <libsystem/io/Connection.h>
#include <libsystem/json/Json.h>

#include <libutils/Optional.h>
#include <libutils/ResultOr.h>

#include <libsettings/Path.h>

namespace settings
{

enum class MessageType : uint8_t
{
    CLIENT_READ,
    CLIENT_WRITE,
    CLIENT_WATCH,
    CLIENT_UNWATCH,

    SERVER_ACK,
    SERVER_VALUE,
    SERVER_NOTIFY,
};

struct Message
{
    MessageType type;
    Optional<Path> path;
    Optional<json::Value> payload;

    static ResultOr<Message> read_from(Connection *connection);

    Result write_to(Connection *connection) const;
};

} // namespace settings
