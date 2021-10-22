#pragma once

#include <libio/Path.h>
#include <libutils/Vec.h>
#include <libwidget/model/TableModel.h>

#include <libfilepicker/model/ArchiveEntryInfo.h>
#include <libfilepicker/model/Navigation.h>

#include <libfile/Archive.h>

namespace FilePicker
{

struct ArchiveListing : public Widget::TableModel
{
private:
    Ref<Navigation> _navigation;
    Ref<Archive> _archive;
    Vec<ArchiveEntryInfo> _entries{};
    Box<Async::Observer<Navigation>> _observer;

public:
    ArchiveListing(Ref<Navigation> navigation, Ref<Archive> archive);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Widget::Var data(int row, int column) override;

    void update() override;

    const ArchiveEntryInfo &info(int index) const;
};

} // namespace FilePicker