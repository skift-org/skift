#pragma once

#include <libgraphic/Icon.h>
#include <libsystem/json/Json.h>
#include <libutils/Observable.h>
#include <libutils/Path.h>
#include <libutils/RefPtr.h>
#include <libutils/Vector.h>

namespace file_manager
{

struct Bookmark
{
    String name;
    RefPtr<Icon> icon;
    Path path;

    Bookmark(const json::Value &value)
    {
        if (value.is(json::OBJECT))
        {
            value.with("name", [&](auto &value) {
                name = value.as_string();
            });

            value.with("icon", [&](auto &value) {
                icon = Icon::get(value.as_string());
            });

            value.with("path", [&](auto &value) {
                path = Path::parse(value.as_string());
            });
        }
    }

    Bookmark(const String &name, const RefPtr<Icon> icon, const Path &path) :
        name(name),
        icon(icon),
        path(path)
    {
    }

    bool
    operator==(const Bookmark &other) const
    {
        return path == other.path;
    }
};

class Bookmarks: public Observable<Bookmarks>, public RefCounted<Bookmarks>
{
private:
    Vector<Bookmark> _bookmarks;

public:
    Bookmarks() {}

    Bookmarks(Vector<Bookmark> &&bookmarks) :
        _bookmarks(bookmarks)
    {
    }

    const Vector<Bookmark> &all()
    {
        return _bookmarks;
    }

    void add(Bookmark &&bookmark)
    {
        _bookmarks.push_back(bookmark);
        did_update();
    }

    void remove(const Path &path)
    {
        for (size_t i = 0; i < _bookmarks.count(); i++)
        {
            if (_bookmarks[i].path == path)
            {
                _bookmarks.remove_index(i);
                did_update();
                return;
            }
        }
    }

    bool has(const Path &path)
    {
        for (size_t i = 0; i < _bookmarks.count(); i++)
        {
            if (_bookmarks[i].path == path)
            {
                return true;
            }
        }

        return false;
    }

    static RefPtr<Bookmarks> load()
    {
        auto raw_bookmarks = json::parse_file("/Configs/file-manager/booksmark.json");

        auto bookmarks = make<Bookmarks>();

        if (!raw_bookmarks.is(json::ARRAY))
        {
            return bookmarks;
        }

        for (size_t i = 0; i < raw_bookmarks.length(); i++)
        {
            bookmarks->add(raw_bookmarks.get(i));
        }

        return bookmarks;
    }
};

} // namespace file_manager
