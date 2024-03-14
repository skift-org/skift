#pragma once

#include "character-data.h"

namespace Web::Dom {

struct Text : public CharacterData {
    using CharacterData::CharacterData;

    virtual NodeType nodeType() override {
        return NodeType::TEXT;
    }
};

} // namespace Web::Dom
