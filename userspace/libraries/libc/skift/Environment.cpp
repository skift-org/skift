#include <libutils/json/Json.h>

static Json::Value *_root = nullptr;

void environment_load(const char *buffer)
{
    if (_root)
    {
        delete _root;
    }
    _root = new Json::Value(move(Json::parse(buffer, strlen(buffer))));
}

Json::Value &environment()
{
    assert(_root);
    return *_root;
}

String environment_copy()
{
    if (_root)
    {
        Prettifier pretty{};
        Json::prettify(pretty, *_root);

        return pretty.finalize();
    }
    else
    {
        return "{}";
    }
}
