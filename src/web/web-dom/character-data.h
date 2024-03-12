#pragma once

#include "node.h"

namespace Web::Dom {

struct CharacterData : public Dom::Node {
    String data;
};

} // namespace Web::Dom
