#pragma once

#include <libutils/HashMap.h>
#include <libutils/String.h>

namespace json
{
enum Type
{
    STRING,
    INTEGER,
    DOUBLE,
    OBJECT,
    ARRAY,
    TRUE,
    FALSE,
    NIL,

    __TYPE_COUNT,
};

struct Value
{
    Type type;

    union {
        char *storage_string;
        int storage_integer;
        double storage_double;
        HashMap<String, Value *> *storage_object;
        Vector<Value *> *storage_array;
    };
};

/* --- Value constructor and destructor --------------------------------- */

// Create a Value of type JSON_STRING  from a cstring by creating a copy.
Value *create_string(const char *string);

// Create a Value of type JSON_STRING from a mallocated string
// and take the ownership of the given string.
Value *create_string_adopt(char *string);

// Create a Value of type JSON_INTEGER.
Value *create_integer(int integer);

// Create a Value of type JSON_DOUBLE.
Value *create_double(double double_);

// Create a Value of type JSON_TRUE or JSON_FALSE.
Value *create_boolean(bool value);

// Create a Value of type JSON_OBJECT which is a HashMap.
Value *create_object();

// Create a Value of type JSON_ARRAY which is a array of Value.
Value *create_array();

// Create a Value of type JSON_TRUE or JSON_FALSE.
Value *create_boolean(bool value);

// Create a Value of type JSON_NULL.
Value *create_nil();

// Destroy a Value and all of its childrens.
void destroy(Value *value);

/* ---  value methodes -------------------------------------------------- */

// The following operation operate on specifique Value type.
// If the Type don't match, an assert will fail.
// So use `is()` before using any of these.

// Check the type of a json value.
bool is(Value *value, Type type);

// Get the value of a string Value.
const char *string_value(Value *value);

// Get the value of a JSON_DOUBLE or JSON_INTEGER has a int
int integer_value(Value *value);

// Get the value of a JSON_DOUBLE or JSON_INTEGER has a double
double double_value(Value *value);

// Return true if the JSON_OBJECT contains the key
bool object_has(Value *object, const String &key);

// Return a Value contained in a JSON_OBJECT by its key. The object keep the ownership of the value
Value *object_get(Value *object, const String &key);

// Put a Value in the json object.
// The object will take the ownership of the value and create a copy of the key.
void object_put(Value *object, const String &key, Value *value);

// Remove a child from a JSON_OBJECT using a key.
void object_remove(Value *object, const String &key);

// Return the length of a JSON_ARRAY
size_t array_length(Value *array);

// Return a child a the specified index. The array keep the ownership of the child.
Value *array_get(Value *array, size_t index);

// Put a Value in the json array at a specified index.
// The array will take the ownership of the value.
void array_put(Value *array, size_t index, Value *value);

// Append a Value a the end of the array.
void array_append(Value *array, Value *value);

// Remove a json value at the specified index and shift all indexes after by -1.
void array_remove(Value *array, size_t index);

/* --- Serialization and Deserialization ------------------------------------ */

// Return a mallocated string representation of the Value tree.
char *stringify(Value *value);

// Return a mallocated string representation of the Value tree
// *WITH* beautiful vt100 colors.
char *prettify(Value *value);

// Parse a json string and return a Value tree.
Value *parse(const char *str, size_t size);

// Parse a json file and return a Value tree.
// Return nullptr on error.
Value *parse_file(const char *path);
} // namespace json
