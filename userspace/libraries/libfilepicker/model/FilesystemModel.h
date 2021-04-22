#pragma once

#include <libio/Directory.h>
#include <libio/Path.h>
#include <libutils/Vector.h>
#include <libwidget/model/TableModel.h>

#include <libfilepicker/model/FileInfo.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

class FilesystemModel : public Widget::TableModel
{
private:
    RefPtr<Navigation> _navigation;
    Vector<FileInfo> _files{};
    OwnPtr<Async::Observer<Navigation>> _observer;
    Callback<bool(IO::Directory::Entry &)> _filter;

public:
    FilesystemModel(RefPtr<Navigation> navigation, Callback<bool(IO::Directory::Entry &)> filter = nullptr);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Variant data(int row, int column) override;

    void update() override;

    const FileInfo &info(int index) const;
};

} // namespace FilePicker