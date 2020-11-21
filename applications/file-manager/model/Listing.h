#pragma once

#include <libutils/Path.h>
#include <libutils/Vector.h>
#include <libwidget/model/TableModel.h>

#include "file-manager/model/Navigation.h"

namespace file_manager
{

struct FileSystemNode
{
    String name;
    FileType type;
    RefPtr<Icon> icon;
    size_t size;
};

class Listing : public TableModel
{
private:
    RefPtr<Navigation> _navigation;
    Vector<FileSystemNode> _files{};
    OwnPtr<Observer<Navigation>> _observer;

public:
    Listing(RefPtr<Navigation> navigation);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Variant data(int row, int column) override;

    void update() override;

    String file_name(int index);

    FileType file_type(int index);
};

} // namespace file_manager
