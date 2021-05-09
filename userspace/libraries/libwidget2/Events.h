#pragma once

#include <libutils/Variant.h>

namespace Widget2
{

struct MouseMove
{
};

struct MousePress
{
};

struct MouseRelease
{
};

struct MouseEnter
{
};

struct MouseLeave
{
};

struct KeyboardPress
{
};

struct KeyboardRelease
{
};

struct Focused
{
};

struct Distracted
{
};

struct Resized
{
};

struct Event : private Utils::Variant<MouseMove, MousePress, MouseRelease, MouseEnter, MouseLeave, MouseLeave, KeyboardPress, KeyboardRelease, Focused, Distracted, Resized>
{
private:
    bool _accepted{};

public:
    void accept() { _accepted = true; }

    void reject() { _accepted = false; }

    template <typename TEvent, typename TCallback>
    void accept(TCallback callback)
    {
        if (is<TEvent>() && !_accepted)
        {
            with<TEvent>(callback);
            _accepted = true;
        }
    }

    template <typename TEvent, typename TCallback>
    void handle(TCallback callback)
    {
        if (is<TEvent>())
        {
            with<TEvent>(callback);
            _accepted = true;
        }
    };
};

} // namespace Widget2
