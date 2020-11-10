#include <libsystem/json/Json.h>
#include <libsystem/process/Environment.h>

static json::Value *_root = nullptr;

void environment_load(const char *buffer)
{
    if (_root)
    {
        delete _root;
    }

    _root = new json::Value(move(json::parse(buffer, strlen(buffer))));
}

json::Value &environment()
{
    assert(_root);
    return *_root;
}

String environment_copy()
{
    if (_root)
    {
        return json::stringify(*_root);
    }
    else
    {
        return "{}";
    }
}
