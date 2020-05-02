#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Directory.h>
#include <libsystem/logger.h>

#include "file-explorer/FileSystemModel.h"

typedef enum
{
    COLUMN_NAME,
    COLUMN_TYPE,
    COLUMN_SIZE,

    __COLUMN_COUNT,
} Column;

static void filesystem_model_update(FileSystemModel *model)
{
    if (model->files)
    {
        list_clear_with_callback(model->files, free);
    }
    else
    {
        model->files = list_create();
    }

    Directory *directory = directory_open(model->current_path, OPEN_READ);

    if (handle_has_error(directory))
    {
        handle_printf_error("Failled to open directory '%s'", model->current_path);
        directory_close(directory);
        return;
    }

    DirectoryEntry entry;
    while (directory_read(directory, &entry) > 0)
    {
        list_pushback_copy(model->files, &entry, sizeof(entry));
    }

    directory_close(directory);
}

static Variant filesystem_model_data(FileSystemModel *model, int row, int column)
{
    DirectoryEntry *entry = NULL;
    assert(list_peekat(model->files, row, (void **)&entry));

    switch (column)
    {
    case COLUMN_NAME:

        if (entry->stat.type == FILE_TYPE_DIRECTORY)
        {
            return variant_with_icon(vstring(entry->name), "folder");
        }
        else
        {
            return variant_with_icon(vstring(entry->name), "file");
        }

    case COLUMN_TYPE:
        switch (entry->stat.type)
        {
        case FILE_TYPE_REGULAR:
            return vstring("Regular file");

        case FILE_TYPE_DIRECTORY:
            return vstring("Directory");

        case FILE_TYPE_DEVICE:
            return vstring("Device");

        default:
            return vstring("Special file");
        }

    case COLUMN_SIZE:
        return vint(entry->stat.size);

    default:
        ASSERT_NOT_REACHED();
    }
}

static int filesystem_model_column_count(void)
{
    return __COLUMN_COUNT;
}

static int filesystem_model_row_count(FileSystemModel *model)
{
    return list_count(model->files);
}

static const char *filesystem_model_column_name(int column)
{
    switch (column)
    {
    case COLUMN_NAME:
        return "Name";
    case COLUMN_TYPE:
        return "Type";
    case COLUMN_SIZE:
        return "Size";

    default:
        ASSERT_NOT_REACHED();
    }
}

void filesystem_model_navigate(FileSystemModel *model, Path *path)
{
    if (model->current_path)
    {
        free(model->current_path);
    }

    model->current_path = path_as_string(path);

    model_update((Model *)model);
}

const char *filesystem_model_filename_by_index(FileSystemModel *model, int index)
{
    if (index >= 0 && index < list_count(model->files))
    {
        DirectoryEntry *entry = NULL;
        assert(list_peekat(model->files, index, (void **)&entry));

        return entry->name;
    }

    return NULL;
}

static void filesystem_model_destroy(FileSystemModel *model)
{
    free(model->current_path);
}

FileSystemModel *filesystem_model_create(const char *current_path)
{
    FileSystemModel *model = __create(FileSystemModel);

    model->current_path = strdup(current_path);

    MODEL(model)->model_update = (ModelUpdateCallback)filesystem_model_update;
    MODEL(model)->model_data = (ModelDataCallback)filesystem_model_data;
    MODEL(model)->model_row_count = (ModelRowCountCallback)filesystem_model_row_count;
    MODEL(model)->model_column_count = (ModelColumnCountCallback)filesystem_model_column_count;
    MODEL(model)->model_column_name = (ModelColumnNameCallback)filesystem_model_column_name;
    MODEL(model)->model_destroy = (ModelDestroyCallback)filesystem_model_destroy;

    model_initialize((Model *)model);

    return model;
}
