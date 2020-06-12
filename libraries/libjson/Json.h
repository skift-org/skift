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

/* --- JsonValue constructor and destructor --------------------------------- */

// Create a JsonValue of type JSON_STRING  from a cstring by creating a copy.
JsonValue *json_create_string(const char *string);

// Create a JsonValue of type JSON_STRING from a mallocated string
// and take the ownership of the given string.
JsonValue *json_create_string_adopt(char *string);

// Create a JsonValue of type JSON_INTEGER.
JsonValue *json_create_integer(int integer);

// Create a JsonValue of type JSON_DOUBLE.
JsonValue *json_create_double(double double_);

// Create a JsonValue of type JSON_TRUE or JSON_FALSE.
JsonValue *json_create_boolean(bool value);

// Create a JsonValue of type JSON_OBJECT which is a HashMap.
JsonValue *json_create_object(void);

// Create a JsonValue of type JSON_ARRAY which is a array of JsonValue.
JsonValue *json_create_array(void);

// Create a JsonValue of type JSON_TRUE or JSON_FALSE.
JsonValue *json_create_boolean(bool value);

// Create a JsonValue of type JSON_NULL.
JsonValue *json_create_null(void);

// Destroy a JsonValue and all of its childrens.
void json_destroy(JsonValue *value);

/* --- Json value methodes -------------------------------------------------- */

// The following operation operate on specifique JsonValue type.
// If the JsonType don't match, an assert will fail.
// So use `json_is()` before using any of these.

// Check the type of a json value.
bool json_is(JsonValue *value, JsonType type);

// Get the value of a string JsonValue.
const char *json_string_value(JsonValue *value);

// Get the value of a JSON_DOUBLE or JSON_INTEGER has a int
int json_integer_value(JsonValue *value);

// Get the value of a JSON_DOUBLE or JSON_INTEGER has a double
double json_double_value(JsonValue *value);

// Return true if the JSON_OBJECT containe the key
bool json_object_has(JsonValue *object, const char *key);

// Return a JsonValue contained in a JSON_OBJECT by its key. The object keep the ownership of the value
JsonValue *json_object_get(JsonValue *object, const char *key);

// Put a JsonValue in the json object.
// The object will take the ownership of the value and create a copy of the key.
void json_object_put(JsonValue *object, const char *key, JsonValue *value);

// Remove a child from a JSON_OBJECT using a key.
void json_object_remove(JsonValue *object, const char *key);

// Return the length of a JSON_ARRAY
size_t json_array_length(JsonValue *array);

// Return a child a the specified index. The array keep the ownership of the child.
JsonValue *json_array_get(JsonValue *array, size_t index);

// Put a JsonValue in the json array at a specified index.
// The array will take the ownership of the value.
void json_array_put(JsonValue *array, size_t index, JsonValue *value);

// Append a JsonValue a the end of the array.
void json_array_append(JsonValue *array, JsonValue *value);

// Remove a json value at the specified index and shift all indexes after by -1.
void json_array_remove(JsonValue *array, size_t index);

/* --- Serialization and Deserialization ------------------------------------ */

// Return a mallocated string representation of the JsonValue tree.
char *json_stringify(JsonValue *value);

// Return a mallocated string representation of the JsonValue tree
// *WITH* beautiful vt100 colors.
char *json_prettify(JsonValue *value);

// Parse a json string and return a JsonValue tree.
JsonValue *json_parse(const char *str, size_t size);

// Parse a json file and return a JsonValue tree.
// Return NULL on error.
JsonValue *json_parse_file(const char *path);
