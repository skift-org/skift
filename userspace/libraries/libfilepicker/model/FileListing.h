#pragma once

#include <libio/Path.h>
#include <libutils/Vector.h>
#include <libwidget/model/TableModel.h>

#include <libfilepicker/model/FileInfo.h>
#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

class FileListing : public Widget::TableModel
{
private:
    RefPtr<Navigation> _navigation;
    Vector<FileInfo> _files{};
    OwnPtr<Async::Observer<Navigation>> _observer;
    String _extension;

public:
    FileListing(RefPtr<Navigation> navigation, String extension = "");

    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Variant data(int row, int column) override;

    void update() override;

    const FileInfo &info(int index) const;
};

} // namespace FilePicker