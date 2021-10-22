#pragma once

#include <libasync/Observable.h>
#include <libutils/Vec.h>

#include <libfilepicker/model/Bookmark.h>

namespace FilePicker
{

struct Bookmarks :
    public Async::Observable<Bookmarks>,
    public Shared<Bookmarks>
{
private:
    Vec<Bookmark> _bookmarks{};

public:
    Bookmarks()
    {
    }

    const Vec<Bookmark> &all() const;

    void add(Bookmark &&bookmark);

    void remove(const IO::Path &path);

    bool has(const IO::Path &path) const;

    static Ref<Bookmarks> load();

    void save();
};

} // namespace FilePicker