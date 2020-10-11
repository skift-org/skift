#include <libjson/Json.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberFormatter.h>

namespace json
{
void stringify_internal(BufferBuilder *builder, Value *value);

Iteration stringify_object(BufferBuilder *builder, String &key, Value *value)
{
    buffer_builder_append_str(builder, "\"");
    buffer_builder_append_str(builder, key.cstring());
    buffer_builder_append_str(builder, "\"");
    buffer_builder_append_str(builder, " : ");
    stringify_internal(builder, value);
    buffer_builder_append_str(builder, ", ");

    return Iteration::CONTINUE;
}

Iteration stringify_array(BufferBuilder *builder, Value *value)
{
    stringify_internal(builder, value);
    buffer_builder_append_str(builder, ", ");

    return Iteration::CONTINUE;
}

void stringify_internal(BufferBuilder *builder, Value *value)
{
    switch (value->type)
    {
    case STRING:
        buffer_builder_append_str(builder, "\"");
        buffer_builder_append_str(builder, value->storage_string);
        buffer_builder_append_str(builder, "\"");
        break;
    case INTEGER:
    {
        char buffer[128];
        format_int(FORMAT_DECIMAL, value->storage_integer, buffer, 128);
        buffer_builder_append_str(builder, buffer);

        break;
    }

#ifndef __KERNEL__

    case DOUBLE:
    {
        char buffer[128];
        format_double(FORMAT_DECIMAL, value->storage_double, buffer, 128);
        buffer_builder_append_str(builder, buffer);

        break;
    }

#endif

    case OBJECT:
        buffer_builder_append_str(builder, "{");

        if (value->storage_object->count() > 0)
        {
            value->storage_object->foreach ([&](auto &key, auto &value) {
                stringify_object(builder, key, value);
                return Iteration::CONTINUE;
            });
            buffer_builder_rewind(builder, 2); // remove the last ", "
        }

        buffer_builder_append_str(builder, "}");
        break;
    case ARRAY:
        buffer_builder_append_str(builder, "[");

        if (value->storage_array->count() > 0)
        {
            value->storage_array->foreach ([&](auto &value) {
                stringify_array(builder, value);
                return Iteration::CONTINUE;
            });

            buffer_builder_rewind(builder, 2); // remove the last ", "
        }

        buffer_builder_append_str(builder, "]");
        break;
    case TRUE:
        buffer_builder_append_str(builder, "true");
        break;
    case FALSE:
        buffer_builder_append_str(builder, "false");
        break;
    case NIL:
        buffer_builder_append_str(builder, "null");
        break;

    default:
        break;
    }
}

char *stringify(Value *value)
{
    BufferBuilder *builder = buffer_builder_create(128);

    stringify_internal(builder, value);

    return buffer_builder_finalize(builder);
}

} // namespace json
