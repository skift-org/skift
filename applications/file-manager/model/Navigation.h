#pragma once

#include <libsystem/process/Process.h>
#include <libutils/Observable.h>
#include <libutils/Path.h>
#include <libutils/Vector.h>

namespace file_manager
{

class Navigation:
    public Observable<Navigation>,
    public RefCounted<Navigation>
{
private:
    Vector<Path> _backward{};
    Path _current;
    Vector<Path> _foreward{};

public:
    enum Direction
    {
        NONE,
        BACKWARD,
        FOREWARD
    };

    const Path &current() { return _current; }

    Navigation() :
        _current(Path::parse(""))
    {
    }

    bool can_go_up()
    {
        return _current.length() > 0;
    }

    void go_up()
    {
        if (can_go_up())
        {
            clear_foreward();
            navigate(_current.dirpath(), BACKWARD);
        }
    }

    bool can_go_backward()
    {
        return _backward.any();
    }

    void go_backward()
    {
        if (can_go_backward())
        {
            navigate(_backward.pop_back(), FOREWARD);
        }
    }

    bool can_go_forward()
    {
        return _foreward.any();
    }

    void go_forward()
    {
        if (can_go_forward())
        {
            navigate(_foreward.pop_back(), BACKWARD);
        }
    }

    void go_home()
    {
        clear_foreward();
        navigate(Path::parse("/User"), BACKWARD);
    }

    void refresh()
    {
        did_update();
    }

    void navigate(String directory) { navigate(Path::parse(directory)); }

    void navigate(Path path)
    {
        clear_foreward();
        navigate(path, BACKWARD);
    }

    void navigate(Path path, Direction record_history)
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

        logger_info("Navigating %s => %s", _current.string().cstring(), path.string().cstring());

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

    void clear_foreward()
    {
        _foreward.clear();
    }
};

} // namespace file_manager
