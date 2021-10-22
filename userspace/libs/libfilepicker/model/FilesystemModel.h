#pragma once

#include <libio/Directory.h>
#include <libio/Path.h>
#include <libutils/Vec.h>
#include <libwidget/model/TableModel.h>

#include <libfilepicker/model/FileInfo.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

struct FilesystemModel : public Widget::TableModel
{
private:
    Ref<Navigation> _navigation;
    Vec<FileInfo> _files{};
    Box<Async::Observer<Navigation>> _observer;
    Func<bool(IO::Directory::Entry &)> _filter;

public:
    FilesystemModel(Ref<Navigation> navigation, Func<bool(IO::Directory::Entry &)> filter = nullptr);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Var data(int row, int column) override;

    void update() override;

    const FileInfo &info(int index) const;
};

} // namespace FilePicker