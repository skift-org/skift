#pragma once

#include <libasync/Observable.h>
#include <libio/Path.h>
#include <libsystem/process/Process.h>
#include <libutils/Vec.h>

namespace FilePicker
{

struct Navigation :
    public Async::Observable<Navigation>,
    public Shared<Navigation>
{
private:
    Vec<IO::Path> _backward{};
    IO::Path _current{};
    Vec<IO::Path> _foreward{};

public:
    enum Direction
    {
        NONE,
        BACKWARD,
        FOREWARD
    };

    const IO::Path &current() const { return _current; }

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

    void navigate(IO::Path path);

    void navigate(IO::Path path, Direction record_history);

    void clear_foreward();
};

} // namespace FilePicker