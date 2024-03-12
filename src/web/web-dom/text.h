#pragma once

#include "character-data.h"

namespace Web::Dom {

struct Text : public CharacterData {
    virtual NodeType nodeType() override {
        return NodeType::TEXT_NODE;
    }
};

} // namespace Web::Dom
