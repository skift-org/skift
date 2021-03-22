#pragma once

#include <libinjection/Entity.h>
#include <libutils/RefPtr.h>

struct Engine : public Injection::Entity
{
    int vroom()
    {
        return 42;
    }
};

struct Car : public Injection::Entity
{
    RefPtr<Engine> _engine;

    INJECTOR(Car(RefPtr<Engine> engine))
        : _engine{engine}
    {
    }

    int vroom()
    {
        return _engine->vroom();
    }
};