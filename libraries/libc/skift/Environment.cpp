#include <libutils/json/Json.h>

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
        Prettifier pretty{};
        json::prettify(pretty, *_root);

        return pretty.finalize();
    }
    else
    {
        return "{}";
    }
}
