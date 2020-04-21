#pragma once

#include <libsystem/path.h>
#include <libsystem/utils/List.h>
#include <libwidget/Model.h>
typedef struct
{
    MODEL_FIELDS

    char *current_path;
    List *files;
} FileSystemModel;

FileSystemModel *filesystem_model_create(const char *current_path);

void filesystem_model_navigate(FileSystemModel *model, Path *path);

const char *filesystem_model_filename_by_index(FileSystemModel *model, int index);