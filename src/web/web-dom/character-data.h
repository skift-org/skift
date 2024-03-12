#pragma once

#include "node.h"

namespace Web::Dom {

struct CharacterData : public Dom::Node {
    String data;

    CharacterData(String data)
        : data(data) {
    }
};

} // namespace Web::Dom
