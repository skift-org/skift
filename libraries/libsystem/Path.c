
#include <libsystem/CString.h>
#include <libsystem/Path.h>
#include <libsystem/io/Stream.h>
#include <libsystem/utils/BufferBuilder.h>

Path *path_create(const char *raw_path)
{
    Path *path = __create(Path);
    path->elements = list_create();

    const char *begin = raw_path;

    path->is_absolute = (raw_path[0] == PATH_SEPARATOR);

    for (size_t i = 0; i <= strlen(raw_path); i++)
    {
        if (raw_path[i] == PATH_SEPARATOR || raw_path[i] == '\0')
        {
            int length = (&raw_path[i] - begin) + 1;

            if (length > 1) // 1 for the null terminator
            {
                char *element = (char *)malloc(length);
                strlcpy(element, begin, length);
                list_pushback(path->elements, element);
            }

            // set the beginning of the next path element and skip the '/'
            begin = &raw_path[i] + 1;
        }
    }

    return path;
}

void path_destroy(Path *path)
{
    list_destroy_with_callback(path->elements, free);
    free(path);
}

const char *path_filename(Path *path)
{
    const char *element;

    if (list_peekback(path->elements, (void **)&element))
    {
        return element;
    }
    else
    {
        return NULL;
    }
}

const char *path_peek_at(Path *path, int index)
{
    const char *element;

    if (list_peekat(path->elements, index, (void **)&element))
    {
        return element;
    }
    else
    {
        return NULL;
    }
}

bool path_is_absolute(Path *path)
{
    return path->is_absolute;
}

bool path_is_relative(Path *path)
{
    return !path->is_absolute;
}

size_t path_element_count(Path *path)
{
    return list_count(path->elements);
}

void path_normalize(Path *path)
{
    List *stack = list_create();

    int depth = 0;

    list_foreach(const char, element, path->elements)
    {
        if ((strcmp(element, "..") == 0) && (depth > 0))
        {
            char *value;
            list_popback(stack, (void **)&value);
            free(value);
            depth--;
        }
        else if (strcmp(element, ".") != 0)
        {
            list_pushback(stack, strdup(element));
            depth++;
        }
    }

    list_destroy_with_callback(path->elements, (ListDestroyElementCallback)free);
    path->elements = stack;
}

void path_push(Path *path, const char *element)
{
    if (!element)
    {
        return;
    }

    if (strlen(element) == 0)
    {
        return;
    }

    list_pushback(path->elements, (char *)element);
}

char *path_pop(Path *path)
{
    char *element = NULL;
    list_popback(path->elements, (void **)&element);
    return element;
}

Path *path_combine(Path *left, Path *right)
{
    Path *p = __create(Path);
    p->elements = list_create();

    // Check if the resulting path is absolute
    if (left != NULL)
    {
        p->is_absolute = left->is_absolute;
    }
    else if (right != NULL)
    {
        p->is_absolute = right->is_absolute;
    }
    else
    {
        p->is_absolute = false;
    }

    // Append the left part of the path
    if (left != NULL)
    {
        p->is_absolute = left->is_absolute;

        list_foreach(const char, i, left->elements)
        {
            path_push(p, strdup(i));
        }
    }

    // Append the right parte of the path
    if (right != NULL)
    {
        list_foreach(const char, i, right->elements)
        {
            path_push(p, strdup(i));
        }
    }

    return p;
}

Path *path_clone(Path *path)
{
    Path *clone = __create(Path);
    clone->elements = list_create();
    clone->is_absolute = path->is_absolute;

    list_foreach(const char, element, path->elements)
    {
        path_push(clone, strdup(element));
    }

    return clone;
}

void path_to_cstring(Path *path, char *buffer, uint size)
{
    buffer[0] = '\0';

    if (path_element_count(path) == 0)
    {
        strnapd(buffer, PATH_SEPARATOR, size);
    }
    else
    {
        list_foreach(const char, i, path->elements)
        {
            const char *element = (const char *)i;
            strnapd(buffer, PATH_SEPARATOR, size);
            strncat(buffer, element, size);
        }
    }
}

char *path_as_string(Path *path)
{
    BufferBuilder *builder = buffer_builder_create(16);

    if (path_element_count(path) == 0)
    {
        buffer_builder_append_chr(builder, PATH_SEPARATOR);
    }
    else
    {
        list_foreach(const char, element, path->elements)
        {
            buffer_builder_append_chr(builder, PATH_SEPARATOR);
            buffer_builder_append_str(builder, element);
        }
    }

    return buffer_builder_finalize(builder);
}

void path_dump(Path *path)
{
    for (size_t i = 0; i < path_element_count(path); i++)
    {
        printf("/%s", path_peek_at(path, i));
    }
}

bool path_equals(Path *left, Path *right)
{
    if (left->is_absolute != right->is_absolute)
    {
        return false;
    }

    if (list_count(left->elements) != list_count(right->elements))
    {
        return false;
    }

    for (int i = 0; i < list_count(left->elements); i++)
    {
        const char *leftstr;
        list_peekat(left->elements, i, (void **)&leftstr);

        const char *rightstr;
        list_peekat(right->elements, i, (void **)&rightstr);

        if (strcmp(leftstr, rightstr) != 0)
        {
            return false;
        }
    }

    return true;
}
