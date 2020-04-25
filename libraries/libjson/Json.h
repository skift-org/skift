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

typedef struct JsonValue
{
    JsonType type;

    union
    {
        char *storage_string;
        int storage_integer;
        double storage_double;
        HashMap *storage_object;
        List *storage_array;
    };
} JsonValue;

JsonValue *json_create_string(char *string);

JsonValue *json_create_string_adopt(char *string);

JsonValue *json_create_integer(int integer);

JsonValue *json_create_double(double double_);

JsonValue *json_create_object(void);

JsonValue *json_create_array(void);

JsonValue *json_create_true(void);

JsonValue *json_create_false(void);

JsonValue *json_create_null(void);

void json_destroy(JsonValue *value);

bool json_is(JsonValue *value, JsonType type);

const char *json_string_value(JsonValue *value);

int json_integer_value(JsonValue *value);

double json_double_value(JsonValue *value);

bool json_object_has(JsonValue *object, const char *key);

JsonValue *json_object_get(JsonValue *object, const char *key);

void json_object_put(JsonValue *object, const char *key, JsonValue *value);

void json_object_remove(JsonValue *object, const char *key);

size_t json_array_lenght(JsonValue *array);

JsonValue *json_array_get(JsonValue *array, size_t index);

void json_array_put(JsonValue *array, size_t index, JsonValue *value);

void json_array_append(JsonValue *array, JsonValue *value);

void json_array_remove(JsonValue *array, size_t index);

char *json_stringify(JsonValue *value);

char *json_prettify(JsonValue *value);

JsonValue *json_parse(const char *str, size_t size);

JsonValue *json_parse_file(const char *path);
