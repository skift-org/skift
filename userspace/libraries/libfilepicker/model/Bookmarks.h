#pragma once

#include <libasync/Observable.h>
#include <libutils/Vector.h>

#include <libfilepicker/model/Bookmark.h>

namespace FilePicker
{

class Bookmarks :
    public Async::Observable<Bookmarks>,
    public RefCounted<Bookmarks>
{
private:
    Vector<Bookmark> _bookmarks{};

public:
    Bookmarks()
    {
    }

    const Vector<Bookmark> &all() const;

    void add(Bookmark &&bookmark);

    void remove(const Path &path);

    bool has(const Path &path) const;

    static RefPtr<Bookmarks> load();

    void save();
};

} // namespace FilePicker