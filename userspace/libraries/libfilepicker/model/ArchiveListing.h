#pragma once

#include <libio/Path.h>
#include <libutils/Vector.h>
#include <libwidget/model/TableModel.h>

#include <libfilepicker/model/ArchiveEntryInfo.h>
#include <libfilepicker/model/Navigation.h>

#include <libfile/Archive.h>

namespace FilePicker
{

class ArchiveListing : public Widget::TableModel
{
private:
    RefPtr<Navigation> _navigation;
    RefPtr<Archive> _archive;
    Vector<ArchiveEntryInfo> _entries{};
    OwnPtr<Async::Observer<Navigation>> _observer;

public:
    ArchiveListing(RefPtr<Navigation> navigation, RefPtr<Archive> archive);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Variant data(int row, int column) override;

    void update() override;

    const ArchiveEntryInfo &info(int index) const;
};

} // namespace FilePicker