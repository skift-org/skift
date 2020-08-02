#pragma once

#include <libsystem/io/Path.h>
#include <libsystem/utils/List.h>
#include <libutils/Vector.h>
#include <libwidget/Model.h>

struct FileSystemNode
{
    char name[FILE_NAME_LENGTH];
    FileType type;
    RefPtr<Icon> icon;
    size_t size;
};

struct FileSystemModel
{
    Model __super;

    char *current_path;
    List *files;
};

FileSystemModel *filesystem_model_create(const char *current_path);

void filesystem_model_navigate(FileSystemModel *model, Path *path);

const char *filesystem_model_filename_by_index(FileSystemModel *model, int index);

FileType filesystem_model_filetype_by_index(FileSystemModel *model, int index);
