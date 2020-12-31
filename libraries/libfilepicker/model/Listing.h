#pragma once

#include <libutils/Path.h>
#include <libutils/Vector.h>
#include <libwidget/model/TableModel.h>

#include <libfilepicker/model/FileInfo.h>
#include <libfilepicker/model/Navigation.h>

namespace filepicker
{

class Listing : public TableModel
{
private:
    RefPtr<Navigation> _navigation;
    Vector<FileInfo> _files{};
    OwnPtr<Observer<Navigation>> _observer;

public:
    Listing(RefPtr<Navigation> navigation);

    int rows() override;

    int columns() override;

    String header(int column) override;

    Variant data(int row, int column) override;

    void update() override;

    const FileInfo &info(int index) const;
};

} // namespace filepicker