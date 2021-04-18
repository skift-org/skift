#pragma once
#include <libmath/Vec2.h>
#include <libutils/Vector.h>

namespace Xml
{

using Version = Math::Vec2i;

class Declaration
{
private:
    Version _version = {1, 0};
    String _encoding = "UTF-8";
    bool _standalone = true;

public:
    const Version &version() const { return _version; }
    const String &encoding() const { return _encoding; }
    bool standalone() const { return _standalone; }
};

} // namespace Xml