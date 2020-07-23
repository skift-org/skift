#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libsystem/math/MinMax.h>
#include <libsystem/system/Logger.h>
#include <libsystem/utils/Vector.h>

struct Vector
{
    size_t element_size;

    void *elements;
    size_t count;
    size_t capacity;
};

Vector *vector_create(size_t element_size, size_t preallocated)
{
    preallocated = MAX(16, preallocated);
    element_size = MAX(1, element_size);

    Vector *vector = __create(Vector);

    vector->element_size = element_size;
    vector->elements = (char *)malloc(preallocated * element_size);
    vector->capacity = preallocated;
    vector->count = 0;

    return vector;
}

void vector_destroy(Vector *vector)
{
    free(vector->elements);
    free(vector);
}

void vector_capacity(Vector *vector, size_t capacity)
{
    if (capacity <= vector->capacity)
        return;

    size_t new_capacity = vector->capacity;

    while (new_capacity <= capacity)
    {
        new_capacity *= 1.25;
    }

    vector->elements = realloc(vector->elements, new_capacity * vector->element_size);
    vector->capacity = new_capacity;
}

void *vector_element_no_check(Vector *vector, size_t index)
{
    return ((char *)vector->elements + (vector->element_size * index));
}

void *vector_element(Vector *vector, size_t index)
{
    assert(index < vector->count);
    return vector_element_no_check(vector, index);
}

void vector_pushback(Vector *vector, void *element)
{
    vector_capacity(vector, vector->count + 1);
    void *destination = vector_element_no_check(vector, vector->count);
    memcpy(destination, element, vector->element_size);
    vector->count++;
}

void vector_popback(Vector *vector, void *element)
{
    void *source = vector_element_no_check(vector, vector->count - 1);
    memcpy(element, source, vector->element_size);
    vector->count--;
}

bool vector_empty(Vector *vector)
{
    return vector->count == 0;
}
