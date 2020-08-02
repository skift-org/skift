#include <libjson/Json.h>
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/process/Process.h>

#include "file-explorer/FileSystemModel.h"

enum Column
{
    COLUMN_NAME,
    COLUMN_TYPE,
    COLUMN_SIZE,

    __COLUMN_COUNT,
};

static auto get_icon_for_node(const char *current_directory, DirectoryEntry *entry)
{
    if (entry->stat.type == FILE_TYPE_DIRECTORY)
    {
        char manifest_path[PATH_LENGTH];
        snprintf(manifest_path, PATH_LENGTH, "%s/%s/manifest.json", current_directory, entry->name);

        JsonValue *root = json_parse_file(manifest_path);

        if (root != nullptr && json_is(root, JSON_OBJECT))
        {
            JsonValue *icon_name = json_object_get(root, "icon");

            if (json_is(icon_name, JSON_STRING))
            {
                auto icon = Icon::get(json_string_value(icon_name));
                json_destroy(root);
                return icon;
            }
        }

        json_destroy(root);
        return Icon::get("folder");
    }
    else if (entry->stat.type == FILE_TYPE_PIPE ||
             entry->stat.type == FILE_TYPE_DEVICE ||
             entry->stat.type == FILE_TYPE_SOCKET)
    {
        return Icon::get("pipe");
    }
    else if (entry->stat.type == FILE_TYPE_TERMINAL)
    {
        return Icon::get("console-network");
    }
    else
    {
        return Icon::get("file");
    }
}

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
        // FIXME: Use message box.
        handle_printf_error(directory, "Failled to open directory '%s'", model->current_path);
        directory_close(directory);
        return;
    }

    DirectoryEntry entry;
    while (directory_read(directory, &entry) > 0)
    {
        FileSystemNode *node = new FileSystemNode{
            .name = {},
            .type = entry.stat.type,
            .icon = get_icon_for_node(model->current_path, &entry),
            .size = entry.stat.size};

        strcpy(node->name, entry.name);

        list_pushback(model->files, node);
    }

    directory_close(directory);
}

static Variant filesystem_model_data(FileSystemModel *model, int row, int column)
{
    FileSystemNode *entry = nullptr;
    assert(list_peekat(model->files, row, (void **)&entry));

    switch (column)
    {
    case COLUMN_NAME:
        return Variant(entry->name).with_icon(entry->icon);

    case COLUMN_TYPE:
        switch (entry->type)
        {
        case FILE_TYPE_REGULAR:
            return Variant("Regular file");

        case FILE_TYPE_DIRECTORY:
            return Variant("Directory");

        case FILE_TYPE_DEVICE:
            return Variant("Device");

        default:
            return Variant("Special file");
        }

    case COLUMN_SIZE:
        return Variant((int)entry->size);

    default:
        ASSERT_NOT_REACHED();
    }
}

static int filesystem_model_column_count()
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
    process_set_directory(model->current_path);
    model_update((Model *)model);
}

const char *filesystem_model_filename_by_index(FileSystemModel *model, int index)
{
    if (index >= 0 && index < list_count(model->files))
    {
        FileSystemNode *entry = nullptr;
        assert(list_peekat(model->files, index, (void **)&entry));

        return entry->name;
    }

    return nullptr;
}

FileType filesystem_model_filetype_by_index(FileSystemModel *model, int index)
{
    if (index >= 0 && index < list_count(model->files))
    {
        FileSystemNode *entry = nullptr;
        assert(list_peekat(model->files, index, (void **)&entry));

        return entry->type;
    }

    return FILE_TYPE_UNKNOWN;
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
