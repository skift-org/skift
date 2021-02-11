#include <libsystem/io_new/File.h>
#include <libutils/json/Json.h>

#include <libfilepicker/model/Bookmarks.h>

namespace filepicker
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

void Bookmarks::save()
{
    json::Value::Array array;

    for (size_t i = 0; i < _bookmarks.count(); i++)
    {
        array.push_back(_bookmarks[i].serialize());
    }

    Prettifier pretty;
    json::prettify(pretty, move(array));

    auto data = pretty.finalize();

    System::File file{"/Configs/file-manager/booksmark.json", OPEN_WRITE | OPEN_CREATE};
    file.write(data.cstring(), data.length());
}

} // namespace filepicker