#pragma once

#include <libsystem/utils/HashMap.h>

typedef enum
{
    JSON_STRING,
    JSON_INTEGER,
    JSON_DOUBLE,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL,

    __JSON_TYPE_COUNT,
} JsonType;

typedef struct
{
    JsonType type;

    union {
        char *storage_string;
        int storage_integer;
        double storage_double;
        HashMap *storage_object;
        List *storage_array;
    };
} JsonValue;

JsonValue *json_create_string(char *string);

JsonValue *json_create_integer(int integer);

JsonValue *json_create_double(double double_);

JsonValue *json_create_object(void);

JsonValue *json_create_array(void);

JsonValue *json_create_true(void);

JsonValue *json_create_false(void);

JsonValue *json_create_null(void);

void json_destroy(JsonValue *value);
