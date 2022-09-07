#pragma once

#include <karm-sys/path.h>
#include <karm-ui/reducer.h>

namespace FileManager {

struct Data {
    Vec<Sys::Path> history;
    size_t currentIndex = 0;

    Data(Sys::Path path)
        : history({path}) {}

    Sys::Path currentPath() const {
        return history[currentIndex];
    }

    bool canGoBack() const {
        return currentIndex > 0;
    }

    bool canGoForward() const {
        return currentIndex + 1 < history.len();
    }

    bool canGoParent() const {
        return currentPath().parent().str().len() > 0;
    }
};

struct GoBack {};

struct GoForward {};

struct GoParent {
    int index;
};

struct GoTo {
    Sys::Path path;
};

struct Refresh {};

struct AddBookmark {};

using Actions = Var<GoBack, GoForward, GoParent, GoTo, Refresh, AddBookmark>;

Data reduce(Data d, Actions action);

using Model = Ui::Model<Data, Actions>;

} // namespace FileManager
