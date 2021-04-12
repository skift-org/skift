#pragma once

#include <libasync/Observable.h>
#include <libsystem/process/Process.h>
#include <libutils/Path.h>
#include <libutils/Vector.h>

namespace FilePicker
{

class Navigation :
    public Async::Observable<Navigation>,
    public RefCounted<Navigation>
{
private:
    Vector<Path> _backward{};
    Path _current{};
    Vector<Path> _foreward{};

public:
    enum Direction
    {
        NONE,
        BACKWARD,
        FOREWARD
    };

    const Path &current() const { return _current; }

    Navigation();

    bool can_go_up();

    void go_up();

    bool can_go_backward();

    void go_backward();

    bool can_go_forward();

    void go_forward();

    void go_home();

    void go_home_dont_record_history();

    void refresh();

    void navigate(String directory);

    void navigate(Path path);

    void navigate(Path path, Direction record_history);

    void clear_foreward();
};

} // namespace FilePicker