#pragma once

#include <libutils/Path.h>
#include <libutils/Vector.h>
#include <libwidget/model/TableModel.h>

struct FileSystemNode
{
    String name;
    FileType type;
    RefPtr<Icon> icon;
    size_t size;
};

class FileSystemModel : public TableModel
{
private:
    String _current_path{};
    Vector<FileSystemNode> _files{};

public:
    FileSystemModel(String path);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Variant data(int row, int column) override;

    void update() override;

    void navigate(Path path);

    String file_name(int index);

    FileType file_type(int index);
};
