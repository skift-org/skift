#include <libio/File.h>
#include <libjson/Json.h>

#include <libfilepicker/model/Bookmarks.h>

namespace FilePicker
{

const Vector<Bookmark> &Bookmarks::all() const
{
    return _bookmarks;
}

void Bookmarks::add(Bookmark &&bookmark)
{
    _bookmarks.push_back(bookmark);
    did_update();
}

void Bookmarks::remove(const Path &path)
{
    for (size_t i = 0; i < _bookmarks.count(); i++)
    {
        if (_bookmarks[i].path() == path)
        {
            _bookmarks.remove_index(i);
            did_update();
            return;
        }
    }
}

bool Bookmarks::has(const Path &path) const
{
    for (size_t i = 0; i < _bookmarks.count(); i++)
    {
        if (_bookmarks[i].path() == path)
        {
            return true;
        }
    }

    return false;
}

RefPtr<Bookmarks> Bookmarks::load()
{
    auto bookmarks = make<Bookmarks>();

    IO::File bookmarks_file{"/Configs/file-manager/booksmark.json", OPEN_READ};

    if (!bookmarks_file.exist())
    {
        return bookmarks;
    }

    auto raw_bookmarks = Json::parse(bookmarks_file);

    if (!raw_bookmarks.is(Json::ARRAY))
    {
        return bookmarks;
    }

    for (size_t i = 0; i < raw_bookmarks.length(); i++)
    {
        bookmarks->add(raw_bookmarks.get(i));
    }

    return bookmarks;
}

void Bookmarks::save()
{
    Json::Value::Array array;

    for (size_t i = 0; i < _bookmarks.count(); i++)
    {
        array.push_back(_bookmarks[i].serialize());
    }

    Prettifier pretty;
    Json::prettify(pretty, move(array));

    auto data = pretty.finalize();

    IO::File file{"/Configs/file-manager/booksmark.json", OPEN_WRITE | OPEN_CREATE};

    if (!file.exist())
    {
        return;
    }

    file.write(data.cstring(), data.length());
}

} // namespace FilePicker