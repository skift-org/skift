#include <libfilepicker/model/Navigation.h>

namespace FilePicker
{

Navigation::Navigation() : _current(Path::parse(""))
{
}

bool Navigation::can_go_up()
{
    return _current.length() > 0;
}

void Navigation::go_up()
{
    if (can_go_up())
    {
        clear_foreward();
        navigate(_current.dirpath(), BACKWARD);
    }
}

bool Navigation::can_go_backward()
{
    return _backward.any();
}

void Navigation::go_backward()
{
    if (can_go_backward())
    {
        navigate(_backward.pop_back(), FOREWARD);
    }
}

bool Navigation::can_go_forward()
{
    return _foreward.any();
}

void Navigation::go_forward()
{
    if (can_go_forward())
    {
        navigate(_foreward.pop_back(), BACKWARD);
    }
}

void Navigation::go_home()
{
    clear_foreward();
    navigate(Path::parse("/User"), BACKWARD);
}

void Navigation::go_home_dont_record_history()
{
    clear_foreward();
    navigate(Path::parse("/User"), NONE);
}

void Navigation::refresh()
{
    did_update();
}

void Navigation::navigate(String directory)
{
    navigate(Path::parse(directory));
}

void Navigation::navigate(Path path)
{
    clear_foreward();
    navigate(path, BACKWARD);
}

void Navigation::navigate(Path path, Direction record_history)
{
    if (path.relative())
    {
        path = Path::join(_current, path);
        path = path.normalized();
    }

    if (_current == path)
    {
        return;
    }

    if (record_history == BACKWARD)
    {
        _backward.push_back(_current);
    }
    else if (record_history == FOREWARD)
    {
        _foreward.push_back(_current);
    }

    _current = path;

    process_set_directory(_current.string().cstring());

    did_update();
}

void Navigation::clear_foreward()
{
    _foreward.clear();
}

} // namespace FilePicker